import { color, withAlpha, brighten } from '../shared/drawing.js';

// Radial-arc "knob" draw callback for <template-pict-slider .drawing=${drawGainKnob}>.
// Signature matches what PetalPictSlider/TemplatePictSlider calls: (ctx, w, h, norm, hovered, aux).
export function drawGainKnob(ctx, w, h, norm, hovered = false) {
    const cx = w / 2;
    const cy = h / 2;
    const radius = Math.min(w, h) / 2 - 4;

    const startAngle = Math.PI * 0.75;
    const endAngle = startAngle + Math.PI * 1.5;
    const valueAngle = startAngle + (endAngle - startAngle) * norm;

    // track
    ctx.beginPath();
    ctx.arc(cx, cy, radius, startAngle, endAngle);
    ctx.strokeStyle = withAlpha(color.grey, 0.6);
    ctx.lineWidth = 3;
    ctx.lineCap = 'round';
    ctx.stroke();

    // value arc
    ctx.beginPath();
    ctx.arc(cx, cy, radius, startAngle, valueAngle);
    ctx.strokeStyle = hovered ? brighten(color.pink) : color.pink;
    ctx.lineWidth = 3;
    ctx.lineCap = 'round';
    ctx.stroke();

    // pointer
    const pointerLength = radius - 6;
    const px = cx + Math.cos(valueAngle) * pointerLength;
    const py = cy + Math.sin(valueAngle) * pointerLength;
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(px, py);
    ctx.strokeStyle = color.lighttan;
    ctx.lineWidth = 2;
    ctx.stroke();
}
