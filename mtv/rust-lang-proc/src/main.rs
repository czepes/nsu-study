//! Текстовый редактор — оболочка (GUI) для языкового процессора.

mod analyzer;
mod highlight;

use std::{cell::Cell, fs, path::PathBuf};

use analyzer::{Kind, Message};
use eframe::egui::{
    self,
    text::{CCursor, CCursorRange},
    Align, Align2, FontId, Id, Key, KeyboardShortcut, Label, Layout, Modifiers, RichText, Sense,
    TextEdit, ViewportCommand,
};
use egui_file_dialog::{DialogState, FileDialog};

const TASK_MD: &str = include_str!("../assets/text/task.md");

const APP_TITLE: &str = "Rust Language Processor";
const EDITOR_ID: &str = "editor";

const HELP: &[(&str, &str)] = &[
    ("Файл › Создать (Ctrl+N)", "Создаёт пустой документ. Если текст изменён, предлагает сохранить изменения."),
    ("Файл › Открыть (Ctrl+O)", "Открывает текстовый файл. Файл можно также перетащить в окно программы."),
    ("Файл › Сохранить (Ctrl+S)", "Сохраняет документ. Для нового документа запрашивает имя файла."),
    ("Файл › Сохранить как (Ctrl+Shift+S)", "Сохраняет документ под новым именем."),
    ("Файл › Выход (Ctrl+Q)", "Закрывает программу; при несохранённых изменениях предлагает сохранить их."),
    ("Правка › Отменить / Повторить (Ctrl+Z / Ctrl+Y)", "Отмена и повтор изменений текста."),
    ("Правка › Вырезать / Копировать / Вставить", "Работа с выделенным фрагментом и буфером обмена (Ctrl+X / Ctrl+C / Ctrl+V)."),
    ("Правка › Удалить", "Удаляет выделенный фрагмент."),
    ("Правка › Выделить все (Ctrl+A)", "Выделяет весь текст."),
    ("Текст", "Открывает окна с информацией (постановка задачи, грамматика и т. д.). Содержимое будет добавлено позже."),
    ("Пуск (F5)", "Запускает анализатор текста. Результаты выводятся в нижнюю область. Щелчок по сообщению об ошибке переносит курсор в позицию ошибки."),
    ("Справка › Вызов справки (F1)", "Открывает это окно."),
    ("Справка › О программе", "Сведения о программе."),
    ("Размер шрифта", "Отдельно для редактора и для области результатов: поля в строке состояния. Для редактора также Ctrl + «+» / Ctrl + «−» / Ctrl + 0."),
    ("Границу областей", "Можно перетаскивать мышью, меняя соотношение размеров редактора и области результатов."),
];

#[derive(Clone, PartialEq)]
enum Pending {
    New,
    Open,
    OpenPath(PathBuf),
    Exit,
}

#[derive(Clone, Copy, PartialEq)]
enum Purpose {
    Open,
    SaveAs,
}

#[derive(Clone, Copy, PartialEq)]
enum Action {
    New,
    Open,
    Save,
    SaveAs,
    Exit,
    Undo,
    Redo,
    Cut,
    Copy,
    Paste,
    Delete,
    SelectAll,
    Run,
    Help,
    About,
    Info(&'static str),
    FontPlus,
    FontMinus,
    FontReset,
}

struct App {
    text: String,
    saved_text: String,
    path: Option<PathBuf>,

    undo: Vec<String>,
    redo: Vec<String>,
    last: String,

    messages: Vec<Message>,
    editor_font: f32,
    output_font: f32,
    cursor_pos: (usize, usize),
    /// Последнее известное выделение (в символах). egui теряет его, когда редактор
    /// теряет фокус (например, при открытии меню), поэтому храним сами.
    sel: Cell<(usize, usize)>,
    status: String,
    last_title: String,

    clipboard: Option<arboard::Clipboard>,
    dialog: FileDialog,
    purpose: Option<Purpose>,
    after_save: Option<Pending>,
    pending: Option<Pending>,
    allow_close: bool,

    show_help: bool,
    show_about: bool,
    info_window: Option<&'static str>,
    md_cache: egui_commonmark::CommonMarkCache,
}

/// Рисует выделение, пока редактор не в фокусе (egui в этом случае его скрывает).
fn paint_inactive_selection(
    ui: &egui::Ui,
    out: &egui::text_edit::TextEditOutput,
    text: &str,
    (a, b): (usize, usize),
    font_size: f32,
) {
    if a >= b {
        return;
    }
    let color = ui.visuals().selection.bg_fill.gamma_multiply(0.45);
    let mut idx = 0usize;
    for line in text.split('\n') {
        let len = line.chars().count();
        let (ls, le) = (idx, idx + len);
        let (s, e) = (a.max(ls), b.min(le + 1)); // +1 — символ перевода строки
        if s < e {
            let r0 = out.galley.pos_from_cursor(CCursor::new(s));
            let r1 = out.galley.pos_from_cursor(CCursor::new(e.min(le)));
            let mut x1 = r1.min.x;
            if e > le {
                x1 += font_size * 0.4;
            }
            let rect = egui::Rect::from_min_max(
                egui::pos2(r0.min.x, r0.min.y),
                egui::pos2(x1.max(r0.min.x + 1.0), r0.max.y),
            )
            .translate(out.galley_pos.to_vec2());
            ui.painter().rect_filled(rect, 0.0, color);
        }
        idx = le + 1;
        if idx > b {
            break;
        }
    }
}

fn byte_idx(s: &str, char_idx: usize) -> usize {
    s.char_indices()
        .nth(char_idx)
        .map(|(b, _)| b)
        .unwrap_or(s.len())
}

fn line_col(s: &str, idx: usize) -> (usize, usize) {
    let (mut line, mut col) = (1, 1);
    for (i, ch) in s.chars().enumerate() {
        if i >= idx {
            break;
        }
        if ch == '\n' {
            line += 1;
            col = 1;
        } else {
            col += 1;
        }
    }
    (line, col)
}

fn common_prefix_chars(a: &str, b: &str) -> usize {
    a.chars().zip(b.chars()).take_while(|(x, y)| x == y).count()
}

impl App {
    fn new() -> Self {
        Self {
            text: String::new(),
            saved_text: String::new(),
            path: None,
            undo: vec![],
            redo: vec![],
            last: String::new(),
            messages: vec![],
            editor_font: EDITOR_FONT_DEFAULT,
            output_font: OUTPUT_FONT_DEFAULT,
            cursor_pos: (1, 1),
            sel: Cell::new((0, 0)),
            status: "Готово".into(),
            last_title: String::new(),
            clipboard: arboard::Clipboard::new().ok(),
            dialog: FileDialog::new(),
            purpose: None,
            after_save: None,
            pending: None,
            allow_close: false,
            show_help: false,
            show_about: false,
            info_window: None,
            md_cache: egui_commonmark::CommonMarkCache::default(),
        }
    }

    fn dirty(&self) -> bool {
        self.text != self.saved_text
    }

    fn doc_name(&self) -> String {
        self.path
            .as_ref()
            .and_then(|p| p.file_name())
            .map(|n| n.to_string_lossy().into_owned())
            .unwrap_or_else(|| "Новый документ".into())
    }

    // ---------- выделение и курсор ----------

    fn editor_id() -> Id {
        Id::from(EDITOR_ID)
    }

    fn selection(&self) -> (usize, usize) {
        let n = self.text.chars().count();
        let (a, b) = self.sel.get();
        (a.min(n), b.min(n))
    }

    fn set_selection(&self, ctx: &egui::Context, a: usize, b: usize) {
        self.sel.set((a, b));
        let id = Self::editor_id();
        let mut st = TextEdit::load_state(ctx, id).unwrap_or_default();
        st.cursor
            .set_char_range(Some(CCursorRange::two(CCursor::new(a), CCursor::new(b))));
        st.store(ctx, id);
        ctx.memory_mut(|m| m.request_focus(id));
    }

    fn jump_to(&self, ctx: &egui::Context, line: usize, col: usize, len: usize) {
        let mut idx = 0;
        for (i, l) in self.text.split('\n').enumerate() {
            if i + 1 == line {
                idx += col.saturating_sub(1).min(l.chars().count());
                break;
            }
            idx += l.chars().count() + 1;
        }
        let total = self.text.chars().count();
        let a = idx.min(total);
        self.set_selection(ctx, a, (a + len).min(total));
    }

    // ---------- документ ----------

    fn set_document(&mut self, ctx: &egui::Context, text: String, path: Option<PathBuf>) {
        self.saved_text = text.clone();
        self.last = text.clone();
        self.text = text;
        self.path = path;
        self.undo.clear();
        self.redo.clear();
        self.messages.clear();
        self.set_selection(ctx, 0, 0);
    }

    fn load(&mut self, ctx: &egui::Context, p: PathBuf) {
        match fs::read_to_string(&p) {
            Ok(s) => {
                self.status = format!("Открыт файл: {}", p.display());
                self.set_document(ctx, s.replace("\r\n", "\n"), Some(p));
            }
            Err(e) => {
                self.messages.push(Message::error_plain(format!(
                    "Не удалось открыть файл: {e}"
                )));
            }
        }
    }

    fn write_to(&mut self, p: PathBuf) -> bool {
        match fs::write(&p, &self.text) {
            Ok(()) => {
                self.status = format!("Сохранено: {}", p.display());
                self.saved_text = self.text.clone();
                self.path = Some(p);
                true
            }
            Err(e) => {
                self.messages.push(Message::error_plain(format!(
                    "Не удалось сохранить файл: {e}"
                )));
                false
            }
        }
    }

    fn start_save_as(&mut self, after: Option<Pending>) {
        self.after_save = after;
        self.purpose = Some(Purpose::SaveAs);
        self.dialog.save_file();
    }

    /// true — файл записан сразу; false — открыт диалог или была ошибка.
    fn save(&mut self, after: Option<Pending>) -> bool {
        match self.path.clone() {
            Some(p) => self.write_to(p),
            None => {
                self.start_save_as(after);
                false
            }
        }
    }

    /// Выполнить действие, но сначала предложить сохранить изменения.
    fn request(&mut self, ctx: &egui::Context, p: Pending) {
        if self.dirty() {
            self.pending = Some(p);
        } else {
            self.execute(ctx, p);
        }
    }

    fn execute(&mut self, ctx: &egui::Context, p: Pending) {
        match p {
            Pending::New => self.set_document(ctx, String::new(), None),
            Pending::Open => {
                self.purpose = Some(Purpose::Open);
                self.dialog.pick_file();
            }
            Pending::OpenPath(path) => self.load(ctx, path),
            Pending::Exit => {
                self.allow_close = true;
                ctx.send_viewport_cmd(ViewportCommand::Close);
            }
        }
    }

    // ---------- правка ----------

    fn replace_selection(&mut self, ctx: &egui::Context, with: &str) {
        let (a, b) = self.selection();
        let (ba, bb) = (byte_idx(&self.text, a), byte_idx(&self.text, b));
        self.text.replace_range(ba..bb, with);
        let pos = a + with.chars().count();
        self.set_selection(ctx, pos, pos);
    }

    fn selected_text(&self) -> String {
        let (a, b) = self.selection();
        self.text[byte_idx(&self.text, a)..byte_idx(&self.text, b)].to_string()
    }

    fn put_clipboard(&mut self, ctx: &egui::Context, s: String) {
        let ok = self
            .clipboard
            .as_mut()
            .map(|cb| cb.set_text(s.clone()).is_ok())
            .unwrap_or(false);
        if !ok {
            ctx.copy_text(s);
        }
    }

    fn do_undo(&mut self, ctx: &egui::Context) {
        if let Some(prev) = self.undo.pop() {
            let pos = common_prefix_chars(&self.text, &prev);
            self.redo
                .push(std::mem::replace(&mut self.text, prev.clone()));
            self.last = prev;
            self.set_selection(ctx, pos, pos);
        }
    }

    fn do_redo(&mut self, ctx: &egui::Context) {
        if let Some(next) = self.redo.pop() {
            let pos = common_prefix_chars(&self.text, &next);
            self.undo
                .push(std::mem::replace(&mut self.text, next.clone()));
            self.last = next;
            self.set_selection(ctx, pos, pos);
        }
    }

    fn perform(&mut self, ctx: &egui::Context, a: Action) {
        match a {
            Action::New => self.request(ctx, Pending::New),
            Action::Open => self.request(ctx, Pending::Open),
            Action::Save => {
                self.save(None);
            }
            Action::SaveAs => self.start_save_as(None),
            Action::Exit => self.request(ctx, Pending::Exit),
            Action::Undo => self.do_undo(ctx),
            Action::Redo => self.do_redo(ctx),
            Action::Copy => {
                let s = self.selected_text();
                if !s.is_empty() {
                    self.put_clipboard(ctx, s);
                }
                let (a, b) = self.selection();
                self.set_selection(ctx, a, b);
            }
            Action::Cut => {
                let s = self.selected_text();
                if !s.is_empty() {
                    self.put_clipboard(ctx, s);
                    self.replace_selection(ctx, "");
                }
            }
            Action::Paste => {
                let s = self.clipboard.as_mut().and_then(|c| c.get_text().ok());
                if let Some(s) = s {
                    self.replace_selection(ctx, &s.replace("\r\n", "\n"));
                }
            }
            Action::Delete => self.replace_selection(ctx, ""),
            Action::SelectAll => {
                let n = self.text.chars().count();
                self.set_selection(ctx, 0, n);
            }
            Action::Run => {
                self.messages = analyzer::analyze(&self.text);
                self.status = "Анализ выполнен".into();
            }
            Action::Help => self.show_help = true,
            Action::About => self.show_about = true,
            Action::Info(t) => self.info_window = Some(t),
            Action::FontPlus => self.editor_font = (self.editor_font + 1.0).min(EDITOR_FONT_MAX),
            Action::FontMinus => self.editor_font = (self.editor_font - 1.0).max(EDITOR_FONT_MIN),
            Action::FontReset => self.editor_font = EDITOR_FONT_DEFAULT,
        }
    }

    // ---------- интерфейс ----------

    fn collect_shortcuts(&self, ctx: &egui::Context) -> Vec<Action> {
        let mut acts = vec![];
        let c = Modifiers::COMMAND;
        let cs = Modifiers::COMMAND | Modifiers::SHIFT;
        ctx.input_mut(|i| {
            let table: [(Modifiers, Key, Action); 14] = [
                (cs, Key::Z, Action::Redo), // раньше Ctrl+Z
                (c, Key::Y, Action::Redo),
                (c, Key::Z, Action::Undo),
                (cs, Key::S, Action::SaveAs),
                (c, Key::S, Action::Save),
                (c, Key::N, Action::New),
                (c, Key::O, Action::Open),
                (c, Key::Q, Action::Exit),
                (Modifiers::NONE, Key::F5, Action::Run),
                (Modifiers::NONE, Key::F1, Action::Help),
                (c, Key::Equals, Action::FontPlus),
                (c, Key::Plus, Action::FontPlus),
                (c, Key::Minus, Action::FontMinus),
                (c, Key::Num0, Action::FontReset),
            ];
            for (m, k, a) in table {
                if i.consume_shortcut(&KeyboardShortcut::new(m, k)) {
                    acts.push(a);
                }
            }
        });
        acts
    }

    fn menu_bar(&self, ui: &mut egui::Ui, acts: &mut Vec<Action>) {
        fn item(ui: &mut egui::Ui, acts: &mut Vec<Action>, label: &str, sc: &str, a: Action) {
            let mut b = egui::Button::new(label);
            if !sc.is_empty() {
                b = b.shortcut_text(sc);
            }
            if ui.add(b).clicked() {
                acts.push(a);
                ui.close();
            }
        }

        egui::MenuBar::new().ui(ui, |ui| {
            ui.menu_button("Файл", |ui| {
                item(ui, acts, "Создать", "Ctrl+N", Action::New);
                item(ui, acts, "Открыть", "Ctrl+O", Action::Open);
                item(ui, acts, "Сохранить", "Ctrl+S", Action::Save);
                item(ui, acts, "Сохранить как", "Ctrl+Shift+S", Action::SaveAs);
                ui.separator();
                item(ui, acts, "Выход", "Ctrl+Q", Action::Exit);
            });
            ui.menu_button("Правка", |ui| {
                item(ui, acts, "Отменить", "Ctrl+Z", Action::Undo);
                item(ui, acts, "Повторить", "Ctrl+Y", Action::Redo);
                ui.separator();
                item(ui, acts, "Вырезать", "Ctrl+X", Action::Cut);
                item(ui, acts, "Копировать", "Ctrl+C", Action::Copy);
                item(ui, acts, "Вставить", "Ctrl+V", Action::Paste);
                item(ui, acts, "Удалить", "Del", Action::Delete);
                ui.separator();
                item(ui, acts, "Выделить все", "Ctrl+A", Action::SelectAll);
            });
            ui.menu_button("Текст", |ui| {
                for t in [
                    "Постановка задачи",
                    "Грамматика",
                    "Классификация грамматики",
                    "Метод анализа",
                    "Тестовый пример",
                    "Список литературы",
                    "Исходный код программы",
                ] {
                    item(ui, acts, t, "", Action::Info(t));
                }
            });
            if ui.button("Пуск").clicked() {
                acts.push(Action::Run);
            }
            ui.menu_button("Справка", |ui| {
                item(ui, acts, "Вызов справки", "F1", Action::Help);
                item(ui, acts, "О программе", "", Action::About);
            });
        });
    }

    fn toolbar(&self, ui: &mut egui::Ui, acts: &mut Vec<Action>) {
        ui.horizontal(|ui| {
            let mut btn = |ui: &mut egui::Ui, icon: &str, tip: &str, on: bool, a: Action| {
                let b = egui::Button::new(
                    RichText::new(icon)
                        .size(18.0)
                        .family(egui::FontFamily::Monospace),
                )
                .min_size(egui::vec2(32.0, 28.0));
                if ui.add_enabled(on, b).on_hover_text(tip).clicked() {
                    acts.push(a);
                }
            };
            btn(ui, "\u{f016}", "Создать", true, Action::New);
            btn(ui, "\u{f115}", "Открыть", true, Action::Open);
            btn(ui, "\u{f0c7}", "Сохранить", true, Action::Save);
            ui.separator();
            btn(
                ui,
                "\u{f0e2}",
                "Отменить",
                !self.undo.is_empty(),
                Action::Undo,
            );
            btn(
                ui,
                "\u{f01e}",
                "Повторить",
                !self.redo.is_empty(),
                Action::Redo,
            );
            btn(ui, "\u{f0c5}", "Копировать", true, Action::Copy);
            btn(ui, "\u{f0c4}", "Вырезать", true, Action::Cut);
            btn(ui, "\u{f0ea}", "Вставить", true, Action::Paste);
            ui.separator();
            btn(ui, "\u{f04b}", "Пуск", true, Action::Run);
            btn(ui, "\u{f059}", "Вызов справки", true, Action::Help);
            btn(ui, "\u{f05a}", "О программе", true, Action::About);
        });
    }

    fn output_area(&self, ui: &mut egui::Ui) -> Option<(usize, usize, usize)> {
        let mut jump = None;
        let fs = self.output_font;
        ui.label(RichText::new("Результаты работы").strong());
        egui::ScrollArea::both()
            .auto_shrink([false, false])
            .show(ui, |ui| {
                egui::Grid::new("messages")
                    .striped(true)
                    .num_columns(4)
                    .spacing([14.0, 4.0])
                    .show(ui, |ui| {
                        for h in ["№", "Строка", "Столбец", "Сообщение"] {
                            ui.label(RichText::new(h).strong().size(fs));
                        }
                        ui.end_row();
                        for (i, m) in self.messages.iter().enumerate() {
                            let color = match m.kind {
                                Kind::Error => egui::Color32::from_rgb(220, 70, 70),
                                Kind::Info => ui.visuals().text_color(),
                            };
                            let cell = |s: String| {
                                Label::new(RichText::new(s).size(fs).color(color))
                                    .sense(Sense::click())
                            };
                            let mut clicked = false;
                            let pos =
                                |v: Option<usize>| v.map(|x| x.to_string()).unwrap_or_default();
                            for s in [(i + 1).to_string(), pos(m.line), pos(m.col), m.text.clone()]
                            {
                                let r = ui.add(cell(s));
                                if m.line.is_some() {
                                    clicked |=
                                        r.on_hover_cursor(egui::CursorIcon::PointingHand).clicked();
                                }
                            }
                            ui.end_row();
                            if clicked {
                                jump = Some((m.line.unwrap_or(1), m.col.unwrap_or(1), m.len));
                            }
                        }
                    });
            });
        jump
    }

    fn editor(&mut self, ui: &mut egui::Ui) {
        let fs = self.editor_font;
        let font = FontId::monospace(fs);
        let dark = ui.visuals().dark_mode;
        let row_h = ui.fonts_mut(|f| f.row_height(&font));
        let rows = ((ui.available_height() / row_h) as usize).max(5);

        let lines = self.text.split('\n').count().max(1);
        let digits = lines.to_string().len().max(2);
        let numbers = (1..=lines)
            .map(|n| format!("{n:>digits$}"))
            .collect::<Vec<_>>()
            .join("\n");
        let num_w = ui.fonts_mut(|f| f.glyph_width(&font, '0')) * (digits as f32 + 1.0);
        let edit_w = (ui.available_width() - num_w - 12.0).max(100.0);

        let mut layouter = |ui: &egui::Ui, text: &dyn egui::TextBuffer, _wrap: f32| {
            ui.fonts_mut(|f| {
                f.layout_job(highlight::layout(
                    text.as_str(),
                    FontId::monospace(fs),
                    dark,
                ))
            })
        };

        let sel = &self.sel;
        let text_for_paint = self.text.clone();
        let mut out = None;
        egui::ScrollArea::both()
            .auto_shrink([false, false])
            .show(ui, |ui| {
                ui.horizontal_top(|ui| {
                    ui.add(
                        Label::new(
                            RichText::new(numbers)
                                .font(font.clone())
                                .color(ui.visuals().weak_text_color()),
                        )
                        .selectable(false),
                    );
                    out = Some(
                        TextEdit::multiline(&mut self.text)
                            .id(Self::editor_id())
                            .font(font.clone())
                            .desired_width(edit_w)
                            .desired_rows(rows)
                            .lock_focus(true)
                            .frame(egui::Frame::NONE)
                            .layouter(&mut layouter)
                            .show(ui),
                    );
                    if let Some(o) = out.as_ref() {
                        let focused = ui.memory(|m| m.has_focus(Self::editor_id()));
                        if let Some(r) = o.cursor_range.as_ref() {
                            let (p, q) = (r.primary.index.0, r.secondary.index.0);
                            sel.set((p.min(q), p.max(q)));
                        } else if !focused {
                            paint_inactive_selection(ui, o, &text_for_paint, sel.get(), fs);
                        }
                    }
                });
            });
        if let Some(r) = out.and_then(|o| o.cursor_range) {
            self.cursor_pos = line_col(&self.text, r.primary.index.0);
        }
    }
}

impl eframe::App for App {
    fn ui(&mut self, ui: &mut egui::Ui, _frame: &mut eframe::Frame) {
        let ctx = &ui.ctx().clone();
        // закрытие окна крестиком
        if ctx.input(|i| i.viewport().close_requested()) && !self.allow_close && self.dirty() {
            ctx.send_viewport_cmd(ViewportCommand::CancelClose);
            self.pending = Some(Pending::Exit);
        }

        // горячие клавиши (до отрисовки редактора, чтобы он не обработал Ctrl+Z сам)
        let mut acts = self.collect_shortcuts(ctx);

        // перетаскивание файла в окно
        let dropped = ctx.input(|i| i.raw.dropped_files.clone());
        if let Some(p) = dropped.into_iter().next().map(|f| f.path().to_path_buf()) {
            self.request(ctx, Pending::OpenPath(p));
        }

        // файловый диалог
        self.dialog.update(ctx);
        if let Some(p) = self.dialog.take_picked() {
            match self.purpose.take() {
                Some(Purpose::Open) => self.load(ctx, p),
                Some(Purpose::SaveAs) if self.write_to(p) => {
                    if let Some(next) = self.after_save.take() {
                        self.execute(ctx, next);
                    }
                }
                _ => {}
            }
        } else if self.purpose.is_some() && matches!(self.dialog.state(), DialogState::Cancelled) {
            self.purpose = None;
            self.after_save = None;
        }

        egui::Panel::top("menu").show(ui, |ui| self.menu_bar(ui, &mut acts));
        egui::Panel::top("toolbar").show(ui, |ui| self.toolbar(ui, &mut acts));

        egui::Panel::bottom("status").show(ui, |ui| {
            ui.horizontal(|ui| {
                ui.label(format!(
                    "Строка: {}, столбец: {}",
                    self.cursor_pos.0, self.cursor_pos.1
                ));
                ui.separator();
                ui.label(
                    self.path
                        .as_ref()
                        .map(|p| p.display().to_string())
                        .unwrap_or_else(|| "Новый документ".into()),
                );
                ui.separator();
                ui.label(&self.status);
                ui.with_layout(Layout::right_to_left(Align::Center), |ui| {
                    ui.add(
                        egui::DragValue::new(&mut self.output_font)
                            .range(OUTPUT_FONT_MIN..=OUTPUT_FONT_MAX)
                            .suffix(" pt"),
                    );
                    ui.label("вывод:");
                    ui.separator();
                    ui.add(
                        egui::DragValue::new(&mut self.editor_font)
                            .range(EDITOR_FONT_MIN..=EDITOR_FONT_MAX)
                            .suffix(" pt"),
                    );
                    ui.label("редактор:");
                    ui.label("Размер шрифта —");
                });
            });
        });

        let mut jump = None;
        egui::Panel::bottom("output")
            .resizable(true)
            .min_size(60.0)
            .default_size(180.0)
            .show(ui, |ui| jump = self.output_area(ui));

        egui::CentralPanel::default().show(ui, |ui| self.editor(ui));

        if let Some((l, c, n)) = jump {
            self.jump_to(ctx, l, c, n);
        }

        // окна
        let mut open = self.show_help;
        egui::Window::new("Справка — руководство пользователя")
            .open(&mut open)
            .default_size([520.0, 420.0])
            .show(ctx, |ui| {
                egui::ScrollArea::vertical().show(ui, |ui| {
                    for (h, d) in HELP {
                        ui.label(RichText::new(*h).strong());
                        ui.label(*d);
                        ui.add_space(6.0);
                    }
                });
            });
        self.show_help = open;

        let mut open = self.show_about;
        egui::Window::new("О программе")
            .open(&mut open)
            .collapsible(false)
            .resizable(false)
            .show(ctx, |ui| {
                ui.heading(APP_TITLE);
                ui.label("Текстовый редактор — оболочка для языкового процессора.");
                ui.label("Версия 0.1");
            });
        self.show_about = open;

        if let Some(t) = self.info_window {
            let mut open = true;
            let cache = &mut self.md_cache;
            egui::Window::new(t)
                .open(&mut open)
                .default_size([560.0, 460.0])
                .show(ctx, |ui| match t {
                    "Постановка задачи" => {
                        egui::ScrollArea::vertical().show(ui, |ui| {
                            egui_commonmark::CommonMarkViewer::new().show(ui, cache, TASK_MD);
                        });
                    }
                    _ => {
                        ui.label("Этот раздел будет реализован позже.");
                    }
                });
            if !open {
                self.info_window = None;
            }
        }

        // запрос на сохранение
        if let Some(p) = self.pending.clone() {
            let mut choice = None;
            egui::Window::new("Сохранение изменений")
                .collapsible(false)
                .resizable(false)
                .anchor(Align2::CENTER_CENTER, [0.0, 0.0])
                .show(ctx, |ui| {
                    ui.label(format!("Сохранить изменения в «{}»?", self.doc_name()));
                    ui.add_space(8.0);
                    ui.horizontal(|ui| {
                        if ui.button("Да").clicked() {
                            choice = Some(true);
                        }
                        if ui.button("Нет").clicked() {
                            choice = Some(false);
                        }
                        if ui.button("Отмена").clicked() {
                            self.pending = None;
                        }
                    });
                });
            match choice {
                Some(true) => {
                    self.pending = None;
                    if self.save(Some(p.clone())) {
                        self.execute(ctx, p);
                    }
                }
                Some(false) => {
                    self.pending = None;
                    self.execute(ctx, p);
                }
                None => {}
            }
        }

        for a in acts {
            self.perform(ctx, a);
        }

        // история для «Отменить/Повторить»
        if self.text != self.last {
            self.undo
                .push(std::mem::replace(&mut self.last, self.text.clone()));
            if self.undo.len() > 500 {
                self.undo.remove(0);
            }
            self.redo.clear();
        }

        // заголовок окна
        let title = format!(
            "{APP_TITLE} — {}{}",
            self.doc_name(),
            if self.dirty() { " *" } else { "" }
        );
        if title != self.last_title {
            ctx.send_viewport_cmd(ViewportCommand::Title(title.clone()));
            self.last_title = title;
        }
    }
}

const OUTPUT_FONT_MIN: f32 = 8.0;
const OUTPUT_FONT_MAX: f32 = 40.0;
const OUTPUT_FONT_DEFAULT: f32 = 15.0;

const EDITOR_FONT_MIN: f32 = 8.0;
const EDITOR_FONT_MAX: f32 = 60.0;
const EDITOR_FONT_DEFAULT: f32 = 30.0;

const EDITOR_FONT: &[u8] = include_bytes!("../assets/font/IosevkaNerdFontMono-Regular.ttf");

fn install_fonts(ctx: &egui::Context) {
    let mut fonts = egui::FontDefinitions::default();
    fonts.font_data.insert(
        "iosevka".to_owned(),
        std::sync::Arc::new(egui::FontData::from_static(EDITOR_FONT)),
    );
    // первым в семействе Monospace; остальные шрифты egui остаются запасными (эмодзи и т. д.)
    fonts
        .families
        .entry(egui::FontFamily::Monospace)
        .or_default()
        .insert(0, "iosevka".to_owned());
    ctx.set_fonts(fonts);
}

fn main() -> eframe::Result {
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_title(APP_TITLE)
            .with_inner_size([980.0, 720.0])
            .with_min_inner_size([640.0, 420.0]),
        ..Default::default()
    };
    eframe::run_native(
        APP_TITLE,
        options,
        Box::new(|cc| {
            install_fonts(&cc.egui_ctx);
            Ok(Box::new(App::new()))
        }),
    )
}
