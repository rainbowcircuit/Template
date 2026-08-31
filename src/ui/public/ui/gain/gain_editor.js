import { LitElement, html, css } from 'lit';
import '../components/slider.js';
import '../components/visualizer.js';
import '../preset/preset_editor.js';
import { getSliderState } from '../../juce.js';
import { drawGainKnob } from './drawing.js';

export class GainEditor extends LitElement {
    static properties = {
        gainNorm: { type: Number },
    };

    static styles = css`
        :host {
            display: flex;
            flex-direction: column;
            height: 100%;
            padding: 20px;
            gap: 16px;
            box-sizing: border-box;
        }

        label {
            margin: 0;
            font-size: 12px;
            font-family: Verdana;
            color: #696969;
            text-align: center;
        }

        .knob-row {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 4px;
        }

        template-visualizer {
            flex: 1;
            min-height: 0;
        }
    `;

    constructor() {
        super();
        this.gainNorm = 0.5;
    }

    firstUpdated() {
        this.gainSlider = getSliderState("gain");
        this.onGainChange = () => { this.gainNorm = this.gainSlider.getNormalisedValue(); };
        this.onGainChange();
        this.gainSlider.valueChangedEvent.addListener(this.onGainChange);
    }

    disconnectedCallback() {
        super.disconnectedCallback();
        if (this.onGainChange) {
            this.gainSlider?.valueChangedEvent.removeListener(this.onGainChange);
        }
    }

    render() {
        return html`
            <preset-editor></preset-editor>

            <div class="knob-row">
                <label>Gain</label>
                <template-pict-slider
                    juceID="gain"
                    .drawing=${drawGainKnob}
                    style="--slider-width: 90px; --slider-height: 90px">
                </template-pict-slider>
                <template-num-slider juceID="gain" suffix=" dB" mode="db" style="--numbox-align: center"></template-num-slider>
            </div>

            <template-visualizer .spin=${0.005 + this.gainNorm * 0.05}></template-visualizer>
        `;
    }
}

customElements.define('gain-editor', GainEditor);
