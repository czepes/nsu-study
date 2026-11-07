//! Заглушка языкового процессора.
//! Позже здесь появятся лексер/парсер вещественных констант Rust.
//! GUI знает только про `analyze` и `Message`.

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum Kind {
    Info,
    Error,
}

/// Одно сообщение в области результатов.
/// Если заданы `line`/`col` (нумерация с 1), щелчок по сообщению
/// переносит курсор редактора в эту позицию и выделяет `len` символов.
#[derive(Clone)]
pub struct Message {
    pub kind: Kind,
    pub line: Option<usize>,
    pub col: Option<usize>,
    pub len: usize,
    pub text: String,
}

impl Message {
    pub fn info(text: impl Into<String>) -> Self {
        Self {
            kind: Kind::Info,
            line: None,
            col: None,
            len: 0,
            text: text.into(),
        }
    }

    pub fn error_plain(text: impl Into<String>) -> Self {
        Self {
            kind: Kind::Error,
            line: None,
            col: None,
            len: 0,
            text: text.into(),
        }
    }

    #[allow(dead_code)] // понадобится, когда появится парсер
    pub fn error(line: usize, col: usize, len: usize, text: impl Into<String>) -> Self {
        Self {
            kind: Kind::Error,
            line: Some(line),
            col: Some(col),
            len,
            text: text.into(),
        }
    }
}

/// Включите, чтобы проверить переход к ошибке по щелчку (демо-сообщение).
const DEMO_ERROR: bool = true;

/// Точка входа для будущего анализатора.
pub fn analyze(source: &str) -> Vec<Message> {
    let mut out = vec![Message::info(format!(
        "Анализатор ещё не реализован. Получено: {} симв., {} стр.",
        source.chars().count(),
        source.split('\n').count()
    ))];

    if DEMO_ERROR {
        // Демо: «ошибка» на первом слове первой непустой строки.
        for (i, line) in source.split('\n').enumerate() {
            let trimmed = line.trim_start();
            if trimmed.is_empty() {
                continue;
            }
            let col = line.chars().count() - trimmed.chars().count() + 1;
            let len = trimmed.chars().take_while(|c| !c.is_whitespace()).count();
            out.push(Message::error(
                i + 1,
                col,
                len,
                "[демо] Пример сообщения об ошибке",
            ));
            break;
        }
    }
    out
}
