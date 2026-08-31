import { LitElement, html, css } from 'lit';
import * as THREE from 'three';

/**
 * TemplateVisualizer hosts a simple Three.js scene (a rotating cube) inside a
 * Lit custom element. It has no JUCE/backend dependency of its own - the
 * `spin` property is a plain number the host page can bind to anything
 * (a slider's normalised value, an audio meter, a constant), which is what
 * gain/gain_editor.js does to make the cube react to the gain parameter.
 */
export class TemplateVisualizer extends LitElement {
    static properties = {
        spin: { type: Number },
    };

    static styles = css`
        :host {
            display: block;
            width: 100%;
            height: 100%;
        }

        #canvas-container {
            position: relative;
            width: 100%;
            height: 100%;
        }

        #canvas-container > canvas {
            position: absolute;
            top: 0;
            left: 0;
        }
    `;

    constructor() {
        super();
        this.spin = 0.01;
    }

    firstUpdated() {
        this.container = this.renderRoot.querySelector('#canvas-container');

        const resizeObserver = new ResizeObserver((entries) => {
            const { width, height } = entries[0].contentRect;
            if (width > 0 && height > 0) {
                resizeObserver.disconnect();
                this.width = width;
                this.height = height;
                this.initializeSpace();
            }
        });
        resizeObserver.observe(this.container);
    }

    initializeSpace() {
        this.scene = new THREE.Scene();
        this.camera = new THREE.PerspectiveCamera(45, this.width / this.height, 0.1, 100);
        this.camera.position.set(0, 0, 4);
        this.camera.lookAt(0, 0, 0);

        this.scene.add(new THREE.AmbientLight(0xffffff, 0.4));
        const directional = new THREE.DirectionalLight(0xffffff, 1.0);
        directional.position.set(2, 3, 4);
        this.scene.add(directional);

        this.geometry = new THREE.BoxGeometry(1.5, 1.5, 1.5);
        this.material = new THREE.MeshStandardMaterial({ color: 0xcb8b93, roughness: 0.4, metalness: 0.1 });
        this.mesh = new THREE.Mesh(this.geometry, this.material);
        this.scene.add(this.mesh);

        this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
        const dpr = window.devicePixelRatio || 1;
        this.renderer.setPixelRatio(dpr);
        this.renderer.setSize(this.width, this.height);
        this.container.appendChild(this.renderer.domElement);

        this.animate();
    }

    animate() {
        this.raf = requestAnimationFrame(() => this.animate());

        this.mesh.rotation.x += this.spin;
        this.mesh.rotation.y += this.spin * 1.3;

        this.renderer.render(this.scene, this.camera);
    }

    disconnectedCallback() {
        super.disconnectedCallback();
        cancelAnimationFrame(this.raf);
        this.geometry?.dispose();
        this.material?.dispose();
        this.renderer?.dispose();
    }

    render() {
        return html`<div id="canvas-container"></div>`;
    }
}

customElements.define('template-visualizer', TemplateVisualizer);
