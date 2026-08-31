import { LitElement, html, css } from 'lit';
import './gain/gain_editor.js';

class App extends LitElement {
    static properties = {
        scale: { type: Number }
    }

    static styles = css`
        :host {
            display: flex;
            width: 100%;
            height: 100%;
            justify-content: center;
            align-items: center;
        }

        #window {
            display: flex;
            width: 480px;
            height: 240px;
            transform-origin: center center;
            justify-content: center;
            align-items: center;
            user-select: none;
            -webkit-touch-callout: none;
            -webkit-user-select: none;
        }

        .window {
            width: 100%;
            height: 100%;
            background-color: #212121;
            border-radius: 10px;
        }
        `;

    constructor(){
        super()
        this.scale = 0.875;
    }

    connectedCallback() {
        super.connectedCallback();
        if (window.__JUCE__) {
            window.__JUCE__.backend.addEventListener("windowSize", (value) => {
                const { width, height } = JSON.parse(value);
                this.scale = Math.min(width / 480, height / 240);
            });
        }
    }

    render(){
        return html`
        <div id="window" style="transform: scale(${this.scale})">
            <gain-editor class="window"></gain-editor>
        </div>
        `
    }
}

customElements.define('main-app', App)
