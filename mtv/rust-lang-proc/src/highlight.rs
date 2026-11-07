//! Простая подсветка синтаксиса: `const`, типы f32/f64, числа, комментарии.

use eframe::egui::{
    text::{LayoutJob, TextFormat},
    Color32, FontId,
};

pub fn layout(text: &str, font: FontId, dark: bool) -> LayoutJob {
    let (def, kw, ty, num, com) = if dark {
        (
            Color32::from_rgb(212, 212, 212),
            Color32::from_rgb(86, 156, 214),
            Color32::from_rgb(78, 201, 176),
            Color32::from_rgb(220, 160, 100),
            Color32::from_rgb(106, 153, 85),
        )
    } else {
        (
            Color32::from_rgb(30, 30, 30),
            Color32::from_rgb(0, 0, 200),
            Color32::from_rgb(0, 128, 128),
            Color32::from_rgb(180, 90, 0),
            Color32::from_rgb(0, 128, 0),
        )
    };
    let fmt = |c: Color32| TextFormat {
        font_id: font.clone(),
        color: c,
        ..Default::default()
    };

    let mut job = LayoutJob::default();
    job.wrap.max_width = f32::INFINITY; // без переноса строк

    let mut plain_start = 0usize;
    let mut i = 0usize;
    while i < text.len() {
        let c = text[i..].chars().next().unwrap();
        let (end, color) = if text[i..].starts_with("//") {
            (
                text[i..].find('\n').map(|p| i + p).unwrap_or(text.len()),
                Some(com),
            )
        } else if c.is_alphabetic() || c == '_' {
            let end = i + text[i..]
                .find(|ch: char| !(ch.is_alphanumeric() || ch == '_'))
                .unwrap_or(text.len() - i);
            let col = match &text[i..end] {
                "const" => Some(kw),
                "f32" | "f64" => Some(ty),
                _ => None,
            };
            (end, col)
        } else if c.is_ascii_digit() {
            let mut end = i;
            let mut prev = ' ';
            for (off, ch) in text[i..].char_indices() {
                let ok = ch.is_ascii_alphanumeric()
                    || ch == '_'
                    || ch == '.'
                    || ((ch == '+' || ch == '-') && (prev == 'e' || prev == 'E'));
                if !ok {
                    break;
                }
                prev = ch;
                end = i + off + ch.len_utf8();
            }
            (end, Some(num))
        } else {
            (i + c.len_utf8(), None)
        };

        if let Some(col) = color {
            if plain_start < i {
                job.append(&text[plain_start..i], 0.0, fmt(def));
            }
            job.append(&text[i..end], 0.0, fmt(col));
            plain_start = end;
        }
        i = end;
    }
    if plain_start < text.len() {
        job.append(&text[plain_start..], 0.0, fmt(def));
    }
    if job.sections.is_empty() {
        job.append("", 0.0, fmt(def));
    }
    job
}
