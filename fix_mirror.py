# --- web_ui.html ---
p = 'external_components/web_editor/web_ui.html'
s = open(p, encoding='utf-8').read()
old = "      ['config.mirror_scale', 'slider', 'Display mirror scale (%)', 10, 300, 5]] },"
new = "      ['config.mirror_scale', 'slider', 'On the Display Now (%)', 10, 100, 5]] },"
assert old in s, 'html slider not found'
s = s.replace(old, new)
old = "      if (typeof value !== 'number' || !Number.isInteger(value) || value < 10 || value > 300)"
new = "      if (typeof value !== 'number' || !Number.isInteger(value) || value < 10 || value > 100)"
assert old in s, 'html validation not found'
s = s.replace(old, new)
old = "    const out = el('output', { class: 'muted small', style: 'margin-left:8px' }, String(val));"
new = "    const out = el('output', { class: 'muted small', style: 'margin-left:8px' }, String(val) + '%');"
assert old in s, 'html output not found'
s = s.replace(old, new)
old = "          oninput: (event) => { const v = Number(event.target.value); out.textContent = String(v); set(v); } }),"
new = "          oninput: (event) => { const v = Number(event.target.value); out.textContent = String(v) + '%'; set(v); } }),"
assert old in s, 'html oninput not found'
s = s.replace(old, new)
open(p, 'w', encoding='utf-8').write(s)

# --- web_ui.h ---
p = 'external_components/web_editor/web_ui.h'
s = open(p, encoding='utf-8').read()
old = "      ['config.mirror_scale', 'int', 'Manual scale (%)', 10, 300],      ['config.pregame_color', 'color', 'Colour'],"
new = "      ['config.mirror_scale', 'int', 'On the Display Now (%)', 10, 100],      ['config.pregame_color', 'color', 'Colour'],"
assert old in s, 'h slider not found'
s = s.replace(old, new)
open(p, 'w', encoding='utf-8').write(s)

print('done')
