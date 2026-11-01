#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="sr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Klizna Kapija — Kontrola</title>
    <style>
        :root {
            --bg-main: #0b1120;
            --bg-card: #151f32;
            --bg-card-inner: #1e293b;
            --border-color: #2a3a52;
            --text-primary: #f1f5f9;
            --text-secondary: #94a3b8;
            --accent-primary: #38bdf8;
            --shadow: 0 8px 24px rgba(0, 0, 0, 0.35);
        }
        body.light-theme {
            --bg-main: #eef2f6;
            --bg-card: #ffffff;
            --bg-card-inner: #f8fafc;
            --border-color: #cbd5e1;
            --text-primary: #0f172a;
            --text-secondary: #475569;
            --accent-primary: #0284c7;
            --shadow: 0 6px 20px rgba(15, 23, 42, 0.08);
        }
        * { margin: 0; padding: 0; box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background-color: var(--bg-main);
            color: var(--text-primary);
            min-height: 100vh;
            padding: 10px;
            transition: background-color 0.2s ease, color 0.2s ease;
        }
        .container { max-width: 540px; margin: 0 auto; }

        /* KOMPAKTNO ZAGLAVLJE + TABOVI U JEDNOM BLOKU */
        .top-bar {
            background: var(--bg-card);
            border: 1px solid var(--border-color);
            border-radius: 16px;
            padding: 10px 12px;
            margin-bottom: 10px;
            box-shadow: var(--shadow);
        }
        .header-row {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 8px;
        }
        .brand { display: flex; align-items: center; gap: 8px; }
        .brand-icon {
            width: 32px; height: 32px; border-radius: 9px;
            background: linear-gradient(135deg, #38bdf8, #6366f1);
            display: flex; align-items: center; justify-content: center;
            font-size: 1.05rem; color: #fff; flex-shrink: 0;
        }
        .brand-title {
            font-size: 1.05rem; font-weight: 700;
            display: flex; align-items: center; gap: 6px;
        }
        .ver-badge {
            font-size: 0.68rem; padding: 1px 6px; border-radius: 999px;
            background: rgba(56, 189, 248, 0.15); color: var(--accent-primary);
            border: 1px solid rgba(56, 189, 248, 0.3); font-weight: 600;
        }
        .header-right { display: flex; align-items: center; gap: 6px; }
        .connection-pill {
            display: flex; align-items: center; gap: 6px;
            background: var(--bg-card-inner); border: 1px solid var(--border-color);
            padding: 4px 10px; border-radius: 999px; font-size: 0.75rem; font-weight: 600;
        }
        .status-led { width: 8px; height: 8px; border-radius: 50%; display: inline-block; }
        .led-connected { background-color: #10b981; box-shadow: 0 0 8px #10b981; }
        .led-disconnected { background-color: #f59e0b; box-shadow: 0 0 8px #f59e0b; animation: blink 1.2s infinite; }
        @keyframes blink { 0%, 100% { opacity: 1; } 50% { opacity: 0.35; } }
        .theme-btn {
            background: var(--bg-card-inner); border: 1px solid var(--border-color);
            color: var(--text-primary); width: 32px; height: 32px; border-radius: 8px;
            cursor: pointer; font-size: 0.95rem; display: flex; align-items: center; justify-content: center;
        }

        /* NAVIGACIJA TABOVA */
        .tabs-nav {
            display: grid; grid-template-columns: 1fr 1fr; gap: 6px;
            background: var(--bg-card-inner); padding: 4px; border-radius: 10px;
        }
        .tab-btn {
            padding: 8px 10px; background: transparent; border: none; border-radius: 7px;
            font-size: 0.85rem; font-weight: 600; color: var(--text-secondary); cursor: pointer;
            display: flex; align-items: center; justify-content: center; gap: 6px;
        }
        .tab-btn.active {
            background: linear-gradient(135deg, #0284c7, #4f46e5);
            color: #ffffff; box-shadow: 0 2px 10px rgba(79, 70, 229, 0.3);
        }
        .tab-content { display: none; }
        .tab-content.active { display: block; }

        /* GLAVNA KARTICA: DUGMAD ODMAH NA VRHU ZA TELEFON */
        .panel {
            background: var(--bg-card); border: 1px solid var(--border-color);
            border-radius: 16px; padding: 12px; margin-bottom: 10px; box-shadow: var(--shadow);
        }

        /* KOMPAKTNA STATUS TRAKA IZNAD DUGMADI */
        .compact-status {
            background: var(--bg-card-inner);
            border: 1px solid var(--border-color);
            border-radius: 12px;
            padding: 10px 12px;
            margin-bottom: 10px;
        }
        .status-top-line {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 6px;
        }
        .status-state-wrap {
            display: flex;
            align-items: baseline;
            gap: 8px;
        }
        .motor-state-big {
            font-size: 1.15rem;
            font-weight: 800;
            letter-spacing: 0.02em;
        }
        .status-stop { color: #94a3b8; }
        .status-otvaram { color: #10b981; }
        .status-zatvaram { color: #f43f5e; }
        .status-error { color: #ef4444; }
        .msg-inline {
            font-size: 0.78rem;
            color: var(--accent-primary);
            font-weight: 600;
        }
        .speed-pill {
            font-size: 0.82rem;
            font-weight: 700;
            color: var(--text-primary);
            background: var(--bg-card);
            padding: 3px 8px;
            border-radius: 6px;
            border: 1px solid var(--border-color);
        }
        .pwm-bar-wrap {
            width: 100%; height: 5px; background: rgba(148, 163, 184, 0.2);
            border-radius: 999px; margin-bottom: 8px; overflow: hidden;
        }
        .pwm-bar-fill {
            height: 100%; width: 0%; background: linear-gradient(90deg, #38bdf8, #10b981);
            border-radius: 999px; transition: width 0.2s ease;
        }
        .chips-row {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 6px;
        }
        .mini-chip {
            background: var(--bg-card);
            border: 1px solid var(--border-color);
            border-radius: 8px;
            padding: 5px 8px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-size: 0.74rem;
            color: var(--text-secondary);
        }
        .badge {
            padding: 2px 6px;
            border-radius: 5px;
            font-size: 0.72rem;
            font-weight: 700;
        }
        .limit-active { background: rgba(16, 185, 129, 0.2); color: #10b981; border: 1px solid rgba(16, 185, 129, 0.4); }
        .limit-inactive { background: rgba(148, 163, 184, 0.12); color: var(--text-secondary); }

        /* 4 GLAVNA DUGMETA (BEZ SKROLOVANJA NA TELEFONU) */
        .control-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            margin-bottom: 10px;
        }
        .ctrl-btn {
            border: none;
            border-radius: 14px;
            padding: 16px 10px;
            color: #ffffff;
            cursor: pointer;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            gap: 4px;
            font-weight: 700;
            transition: transform 0.12s ease, filter 0.15s ease;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
        }
        .ctrl-btn:active { transform: scale(0.97); }
        .ctrl-icon { font-size: 1.45rem; line-height: 1; }
        .ctrl-title { font-size: 0.98rem; text-transform: uppercase; letter-spacing: 0.02em; }
        .ctrl-desc { font-size: 0.7rem; opacity: 0.88; font-weight: 500; }
        .btn-open { background: linear-gradient(135deg, #10b981, #059669); }
        .btn-close { background: linear-gradient(135deg, #f43f5e, #e11d48); }
        .btn-pedestrian { background: linear-gradient(135deg, #6366f1, #4f46e5); }
        .btn-stop { background: linear-gradient(135deg, #f59e0b, #d97706); color: #0f172a; }

        /* ZAKLJUČAVANJE ZIDNOG TASTERA I AUTO-CLOSE U ISTOM EKRANU */
        .quick-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 8px;
        }
        .setting-box {
            background: var(--bg-card-inner);
            border: 1px solid var(--border-color);
            border-radius: 12px;
            padding: 9px 10px;
            display: flex;
            flex-direction: column;
            gap: 6px;
        }
        .setting-box-label {
            font-size: 0.73rem;
            color: var(--text-secondary);
            font-weight: 600;
            text-transform: uppercase;
            letter-spacing: 0.03em;
        }
        select, input[type="number"] {
            background: var(--bg-main); color: var(--text-primary); border: 1px solid var(--border-color);
            border-radius: 8px; padding: 7px 8px; font-size: 0.85rem; font-weight: 600; outline: none; width: 100%;
        }
        .btn-wall-lock {
            width: 100%;
            padding: 8px 10px;
            border-radius: 8px;
            font-size: 0.8rem;
            font-weight: 700;
            cursor: pointer;
            border: 1px solid transparent;
            transition: all 0.2s ease;
            text-align: center;
        }
        .wall-enabled {
            background: rgba(16, 185, 129, 0.18);
            color: #10b981;
            border-color: rgba(16, 185, 129, 0.45);
        }
        .wall-locked {
            background: rgba(244, 63, 94, 0.2);
            color: #f43f5e;
            border-color: rgba(244, 63, 94, 0.5);
        }

        /* TAB 2: PODEŠAVANJE MOTORA */
        .panel-header {
            display: flex; justify-content: space-between; align-items: center;
            margin-bottom: 12px; padding-bottom: 8px; border-bottom: 1px solid var(--border-color);
        }
        .panel-title { font-size: 0.88rem; font-weight: 700; text-transform: uppercase; letter-spacing: 0.04em; color: var(--text-secondary); }
        .settings-info {
            font-size: 0.82rem; color: var(--text-secondary); margin-bottom: 14px; line-height: 1.4;
            background: var(--bg-card-inner); padding: 10px 12px; border-radius: 10px; border-left: 3px solid var(--accent-primary);
        }
        .profiles-grid { display: grid; grid-template-columns: 1fr; gap: 12px; margin-bottom: 14px; }
        .profile-box { background: var(--bg-card-inner); border: 1px solid var(--border-color); border-radius: 12px; padding: 14px; }
        .profile-open { border-top: 4px solid #10b981; }
        .profile-close { border-top: 4px solid #f43f5e; }
        .profile-ped { border-top: 4px solid #6366f1; }
        .profile-box h3 { font-size: 0.92rem; margin-bottom: 10px; }
        .param-row {
            display: flex; justify-content: space-between; align-items: center;
            margin-bottom: 8px; padding-bottom: 6px; border-bottom: 1px dashed rgba(148, 163, 184, 0.15);
        }
        .param-row:last-child { margin-bottom: 0; padding-bottom: 0; border-bottom: none; }
        .param-row label { font-size: 0.82rem; color: var(--text-secondary); font-weight: 500; }
        .param-row input { width: 82px; text-align: right; }
        .actions-footer { display: flex; gap: 10px; }
        .btn-save {
            flex: 2; background: linear-gradient(135deg, #0284c7, #4f46e5); color: #ffffff;
            border: none; border-radius: 10px; padding: 13px 16px; font-size: 0.9rem; font-weight: 700; cursor: pointer;
        }
        .btn-reset {
            flex: 1; background: var(--bg-card-inner); color: var(--text-secondary);
            border: 1px solid var(--border-color); border-radius: 10px; padding: 13px 12px; font-size: 0.82rem; font-weight: 600; cursor: pointer;
        }
        .toast {
            position: fixed; bottom: 16px; left: 50%; transform: translate(-50%, 12px);
            background: #10b981; color: #ffffff; padding: 10px 16px; border-radius: 10px;
            font-size: 0.85rem; font-weight: 600; box-shadow: 0 8px 20px rgba(0, 0, 0, 0.4);
            opacity: 0; pointer-events: none; transition: all 0.22s ease; z-index: 999; white-space: nowrap;
        }
        .toast.show { opacity: 1; transform: translate(-50%, 0); }
    </style>
</head>
<body>
    <div class="container">
        <!-- KOMPAKTNO ZAGLAVLJE + KARTICE -->
        <header class="top-bar">
            <div class="header-row">
                <div class="brand">
                    <div class="brand-icon">🚪</div>
                    <div class="brand-title">
                        Klizna Kapija <span class="ver-badge" id="fw-ver">v5.1</span>
                    </div>
                </div>
                <div class="header-right">
                    <div class="connection-pill">
                        <span class="status-led led-disconnected" id="status-led"></span>
                        <span id="connection-text">Povezivanje...</span>
                    </div>
                    <button class="theme-btn" id="theme-toggle-btn" onclick="toggleTheme()" title="Promeni temu">☀️</button>
                </div>
            </div>

            <nav class="tabs-nav">
                <button class="tab-btn active" id="btn-tab-control" onclick="switchTab('tab-control')">
                    <span>🎮</span> Kontrola Kapije
                </button>
                <button class="tab-btn" id="btn-tab-settings" onclick="switchTab('tab-settings')">
                    <span>⚙️</span> Podešavanje Motora
                </button>
            </nav>
        </header>

        <!-- ================= KARTICA 1: KONTROLA (SVE STAJE NA 1 EKRAN TELEFONA) ================= -->
        <section id="tab-control" class="tab-content active">
            <div class="panel">
                <!-- 1. KOMPAKTNI STATUS IZNAD DUGMADI -->
                <div class="compact-status">
                    <div class="status-top-line">
                        <div class="status-state-wrap">
                            <span id="motor-status" class="motor-state-big status-stop">STOP</span>
                            <span id="last-message" class="msg-inline">Spremno</span>
                            <span id="mode-indicator" style="display:none;">Mirovanje</span>
                        </div>
                        <div class="speed-pill">
                            <span id="speed">0</span> PWM (<span id="speed-pct">0%</span>)
                        </div>
                    </div>

                    <div class="pwm-bar-wrap">
                        <div class="pwm-bar-fill" id="pwm-bar"></div>
                    </div>

                    <div class="chips-row">
                        <div class="mini-chip">
                            <span>Otv.</span>
                            <span id="limit-open" class="badge limit-inactive">NE</span>
                        </div>
                        <div class="mini-chip">
                            <span>Zat.</span>
                            <span id="limit-close" class="badge limit-inactive">NE</span>
                        </div>
                        <div class="mini-chip">
                            <span>Auto</span>
                            <span id="auto-close" class="badge limit-inactive">2m</span>
                        </div>
                    </div>
                </div>

                <!-- 2. GLAVNA DUGMAD ZA UPRAVLJANJE (ODMAH POD PRSTOM BEZ SKROLOVANJA) -->
                <div class="control-grid">
                    <button onclick="sendCmd('open')" class="ctrl-btn btn-open">
                        <span class="ctrl-icon">🔓</span>
                        <span class="ctrl-title">Otvori</span>
                        <span class="ctrl-desc">Pun hod otvaranja</span>
                    </button>
                    <button onclick="sendCmd('close')" class="ctrl-btn btn-close">
                        <span class="ctrl-icon">🔒</span>
                        <span class="ctrl-title">Zatvori</span>
                        <span class="ctrl-desc">Pun hod zatvaranja</span>
                    </button>
                    <button onclick="sendCmd('pedestrian')" class="ctrl-btn btn-pedestrian">
                        <span class="ctrl-icon">🚶</span>
                        <span class="ctrl-title">Pešački Mod</span>
                        <span class="ctrl-desc">Delimično otvori (~1m)</span>
                    </button>
                    <button onclick="sendCmd('stop')" class="ctrl-btn btn-stop">
                        <span class="ctrl-icon">⏹️</span>
                        <span class="ctrl-title">Stop</span>
                        <span class="ctrl-desc">Trenutno zaustavljanje</span>
                    </button>
                </div>

                <!-- 3. ZAKLJUČAVANJE ZIDNOG TASTERA I AUTO-ZATVARANJE -->
                <div class="quick-grid">
                    <div class="setting-box">
                        <span class="setting-box-label">Zidni Taster (Zaključavanje)</span>
                        <button id="toggle-mode-btn" onclick="toggleManualBtn()" class="btn-wall-lock wall-enabled">🔓 TASTER UKLJUČEN</button>
                    </div>
                    <div class="setting-box">
                        <label class="setting-box-label" for="auto-close-select">Auto-Zatvaranje</label>
                        <select id="auto-close-select" onchange="saveAllSettings()">
                            <option value="off">Isključeno</option>
                            <option value="1">1 minut</option>
                            <option value="2" selected>2 minuta</option>
                            <option value="3">3 minuta</option>
                            <option value="4">4 minuta</option>
                            <option value="5">5 minuta</option>
                            <option value="10">10 minuta</option>
                        </select>
                    </div>
                </div>
            </div>
        </section>

        <!-- ================= KARTICA 2: PODEŠAVANJE MOTORA ================= -->
        <section id="tab-settings" class="tab-content">
            <div class="panel">
                <div class="panel-header">
                    <span class="panel-title">Kalibracija Motora Brisača (Soft-Start & Soft-Stop)</span>
                </div>
                <p class="settings-info">
                    Nezavisno podešavanje PWM snage, zaleta, trajanja pune brzine i usporavanja pred kraj za otvaranje i zatvaranje.
                </p>

                <div class="profiles-grid">
                    <div class="profile-box profile-open">
                        <h3>🟢 OTVARANJE (Smer 1)</h3>
                        <div class="param-row">
                            <label for="o-max-spd">Puna brzina (PWM 50–255):</label>
                            <input type="number" id="o-max-spd" min="50" max="255" value="210">
                        </div>
                        <div class="param-row">
                            <label for="o-min-spd">Završna spora brzina (PWM):</label>
                            <input type="number" id="o-min-spd" min="30" max="180" value="80">
                        </div>
                        <div class="param-row">
                            <label for="o-accel">Meki start / Ubrzanje (sek):</label>
                            <input type="number" id="o-accel" min="0.5" max="15" step="0.5" value="4.0">
                        </div>
                        <div class="param-row">
                            <label for="o-run">Trajanje pune brzine (sek):</label>
                            <input type="number" id="o-run" min="1" max="60" step="0.5" value="11.0">
                        </div>
                        <div class="param-row">
                            <label for="o-timeout">Sigurnosni Timeout (sek):</label>
                            <input type="number" id="o-timeout" min="5" max="90" value="28">
                        </div>
                    </div>

                    <div class="profile-box profile-close">
                        <h3>🔴 ZATVARANJE (Smer 2)</h3>
                        <div class="param-row">
                            <label for="c-max-spd">Puna brzina (PWM 50–255):</label>
                            <input type="number" id="c-max-spd" min="50" max="255" value="175">
                        </div>
                        <div class="param-row">
                            <label for="c-min-spd">Završna spora brzina (PWM):</label>
                            <input type="number" id="c-min-spd" min="30" max="180" value="70">
                        </div>
                        <div class="param-row">
                            <label for="c-accel">Meki start / Ubrzanje (sek):</label>
                            <input type="number" id="c-accel" min="0.5" max="15" step="0.5" value="3.0">
                        </div>
                        <div class="param-row">
                            <label for="c-run">Trajanje pune brzine (sek):</label>
                            <input type="number" id="c-run" min="1" max="60" step="0.5" value="8.5">
                        </div>
                        <div class="param-row">
                            <label for="c-timeout">Sigurnosni Timeout (sek):</label>
                            <input type="number" id="c-timeout" min="5" max="90" value="22">
                        </div>
                    </div>

                    <div class="profile-box profile-ped">
                        <h3>🚶 PEŠAČKI PROLAZ (Kanal C & Web Dugme)</h3>
                        <div class="param-row">
                            <label for="p-open-time">Trajanje otvaranja za pešaka (sek):</label>
                            <input type="number" id="p-open-time" min="1" max="20" step="0.5" value="4.5">
                        </div>
                        <div class="param-row">
                            <label for="p-wait-time">Zadržavanje pre zatvaranja (sek):</label>
                            <input type="number" id="p-wait-time" min="3" max="120" step="1" value="15">
                        </div>
                    </div>
                </div>

                <div class="actions-footer">
                    <button onclick="saveAllSettings()" class="btn-save">💾 SAČUVAJ PODEŠAVANJA U FLASH</button>
                    <button onclick="resetDefaultSettings()" class="btn-reset">↺ Fabričke vrednosti</button>
                </div>
            </div>
        </section>
    </div>

    <div id="toast" class="toast">Podešavanja sačuvana!</div>

    <script>
        let ws = null;
        let isWallButtonEnabled = true;
        let simTimer = null;
        let isConnectedToESP = false;

        function showToast(msg, isError) {
            const t = document.getElementById('toast');
            t.textContent = msg;
            t.style.background = isError ? '#ef4444' : '#10b981';
            t.classList.add('show');
            setTimeout(() => t.classList.remove('show'), 2600);
        }

        function updateWallLockUI(enabled) {
            isWallButtonEnabled = Boolean(enabled);
            const btn = document.getElementById('toggle-mode-btn');
            if (isWallButtonEnabled) {
                btn.textContent = '🔓 TASTER UKLJUČEN';
                btn.className = 'btn-wall-lock wall-enabled';
            } else {
                btn.textContent = '🔒 ZAKLJUČAN (OFF)';
                btn.className = 'btn-wall-lock wall-locked';
            }
        }

        function toggleTheme() {
            document.body.classList.toggle('light-theme');
            const isLight = document.body.classList.contains('light-theme');
            document.getElementById('theme-toggle-btn').textContent = isLight ? '🌙' : '☀️';
        }

        function switchTab(tabId) {
            document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
            document.querySelectorAll('.tab-content').forEach(content => content.classList.remove('active'));
            document.getElementById(tabId === 'tab-control' ? 'btn-tab-control' : 'btn-tab-settings').classList.add('active');
            document.getElementById(tabId).classList.add('active');
        }

        function applyStatusData(data) {
            if (data.version) document.getElementById('fw-ver').textContent = data.version;
            const motorEl = document.getElementById('motor-status');
            motorEl.textContent = data.motorStatus;
            motorEl.className = 'motor-state-big status-' + data.motorStatus.toLowerCase();

            const spd = parseInt(data.speed) || 0;
            const pct = Math.round((spd / 255) * 100);
            document.getElementById('speed').textContent = spd;
            document.getElementById('speed-pct').textContent = pct + '%';
            document.getElementById('pwm-bar').style.width = pct + '%';

            const openEl = document.getElementById('limit-open');
            openEl.textContent = data.limitOpen ? 'DA' : 'NE';
            openEl.className = 'badge ' + (data.limitOpen ? 'limit-active' : 'limit-inactive');

            const closeEl = document.getElementById('limit-close');
            closeEl.textContent = data.limitClose ? 'DA' : 'NE';
            closeEl.className = 'badge ' + (data.limitClose ? 'limit-active' : 'limit-inactive');

            document.getElementById('auto-close').textContent = data.autoClose || 'OFF';
            if (data.lastMessage) document.getElementById('last-message').textContent = data.lastMessage;

            if (typeof data.toggleMode !== 'undefined') {
                updateWallLockUI(data.toggleMode);
            }
        }

        function connectWS() {
            const host = window.location.hostname;
            if (!host || window.location.protocol === 'file:' || host.includes('googleusercontent') || host.includes('localhost')) {
                document.getElementById('status-led').className = 'status-led led-connected';
                document.getElementById('connection-text').textContent = 'Pregled';
                return;
            }
            try {
                ws = new WebSocket('ws://' + host + ':81/');
                ws.onopen = function() {
                    isConnectedToESP = true;
                    document.getElementById('status-led').className = 'status-led led-connected';
                    document.getElementById('connection-text').textContent = 'Povezano';
                    loadSettings();
                };
                ws.onclose = function() {
                    isConnectedToESP = false;
                    document.getElementById('status-led').className = 'status-led led-disconnected';
                    document.getElementById('connection-text').textContent = 'Prekid...';
                    setTimeout(connectWS, 2500);
                };
                ws.onmessage = function(event) {
                    try { applyStatusData(JSON.parse(event.data)); } catch (e) {}
                };
            } catch (err) {}
        }

        function sendCmd(cmd) {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ command: cmd }));
                showToast('Komanda: ' + cmd.toUpperCase());
                return;
            }
            if (simTimer) clearInterval(simTimer);
            const modeInd = document.getElementById('mode-indicator');
            if (cmd === 'stop') {
                modeInd.textContent = 'Zaustavljeno';
                applyStatusData({
                    motorStatus: 'STOP', speed: 0, limitOpen: false, limitClose: false,
                    autoClose: document.getElementById('auto-close').textContent,
                    toggleMode: isWallButtonEnabled, lastMessage: 'Zaustavljeno'
                });
                showToast('Motor zaustavljen');
                return;
            }
            const targetMax = cmd === 'close'
                ? parseInt(document.getElementById('c-max-spd').value) || 175
                : parseInt(document.getElementById('o-max-spd').value) || 210;
            let curSpd = 40;
            const statusLabel = cmd === 'close' ? 'ZATVARAM' : 'OTVARAM';
            modeInd.textContent = cmd === 'pedestrian' ? 'Pešački (~1m)' : statusLabel;
            showToast(cmd === 'pedestrian' ? 'Pešački mod (~1m)' : 'Pokrenuto: ' + statusLabel);
            simTimer = setInterval(() => {
                if (curSpd < targetMax) curSpd = Math.min(curSpd + 25, targetMax);
                applyStatusData({
                    motorStatus: statusLabel, speed: curSpd, limitOpen: false, limitClose: false,
                    autoClose: document.getElementById('auto-close').textContent,
                    toggleMode: isWallButtonEnabled,
                    lastMessage: cmd === 'pedestrian' ? 'Pešački mod...' : (statusLabel + '...')
                });
            }, 250);
        }

        function toggleManualBtn() {
            updateWallLockUI(!isWallButtonEnabled);
            saveAllSettings();
            showToast(isWallButtonEnabled ? 'Zidni taster UKLJUČEN' : 'Zidni taster ZAKLJUČAN (isključen)', !isWallButtonEnabled);
        }

        function loadSettings() {
            if (!window.location.hostname || !isConnectedToESP) return;
            const apiUrl = window.location.origin + '/api/settings';
            fetch(apiUrl)
                .then(r => r.json())
                .then(s => {
                    document.getElementById('auto-close-select').value = s.autoCloseEnabled ? s.autoCloseMinutes : 'off';
                    updateWallLockUI(s.toggleMode);
                    document.getElementById('o-max-spd').value = s.open.maxSpeed;
                    document.getElementById('o-min-spd').value = s.open.minSpeed;
                    document.getElementById('o-accel').value   = s.open.accelTime;
                    document.getElementById('o-run').value     = s.open.runTime;
                    document.getElementById('o-timeout').value = s.open.maxTimeout;
                    document.getElementById('c-max-spd').value = s.close.maxSpeed;
                    document.getElementById('c-min-spd').value = s.close.minSpeed;
                    document.getElementById('c-accel').value   = s.close.accelTime;
                    document.getElementById('c-run').value     = s.close.runTime;
                    document.getElementById('c-timeout').value = s.close.maxTimeout;
                    if (s.pedestrianOpenTime) document.getElementById('p-open-time').value = s.pedestrianOpenTime;
                    if (s.pedestrianWaitTime) document.getElementById('p-wait-time').value = s.pedestrianWaitTime;
                })
                .catch(() => {});
        }

        function saveAllSettings() {
            const acVal = document.getElementById('auto-close-select').value;
            const payload = {
                saveSettings: true,
                autoCloseEnabled: acVal !== 'off',
                autoCloseMinutes: acVal === 'off' ? 2 : parseInt(acVal),
                toggleMode: isWallButtonEnabled,
                pedestrianOpenTime: parseFloat(document.getElementById('p-open-time').value) || 4.5,
                pedestrianWaitTime: parseFloat(document.getElementById('p-wait-time').value) || 15,
                open: {
                    maxSpeed: parseInt(document.getElementById('o-max-spd').value),
                    minSpeed: parseInt(document.getElementById('o-min-spd').value),
                    accelTime: parseFloat(document.getElementById('o-accel').value),
                    runTime: parseFloat(document.getElementById('o-run').value),
                    maxTimeout: parseInt(document.getElementById('o-timeout').value)
                },
                close: {
                    maxSpeed: parseInt(document.getElementById('c-max-spd').value),
                    minSpeed: parseInt(document.getElementById('c-min-spd').value),
                    accelTime: parseFloat(document.getElementById('c-accel').value),
                    runTime: parseFloat(document.getElementById('c-run').value),
                    maxTimeout: parseInt(document.getElementById('c-timeout').value)
                }
            };
            document.getElementById('auto-close').textContent = acVal === 'off' ? 'OFF' : (acVal + 'm');
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify(payload));
            }
            showToast('Podešavanja sačuvana u Flash!');
        }

        function resetDefaultSettings() {
            document.getElementById('o-max-spd').value = 210;
            document.getElementById('o-min-spd').value = 80;
            document.getElementById('o-accel').value   = 4.0;
            document.getElementById('o-run').value     = 11.0;
            document.getElementById('o-timeout').value = 28;
            document.getElementById('c-max-spd').value = 175;
            document.getElementById('c-min-spd').value = 70;
            document.getElementById('c-accel').value   = 3.0;
            document.getElementById('c-run').value     = 8.5;
            document.getElementById('c-timeout').value = 22;
            document.getElementById('p-open-time').value = 4.5;
            document.getElementById('p-wait-time').value = 15;
            showToast('Vraćene fabričke vrednosti — klikni Sačuvaj');
        }

        connectWS();
    </script>
</body>
</html>
)rawliteral";

#endif
