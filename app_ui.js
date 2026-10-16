(() => {
  const tabs = ["join", "soundpad", "spam", "video", "users"];
  const send = (action, payload = {}) => {
    if (window.chrome?.webview) window.chrome.webview.postMessage({ action, ...payload });
  };
  const byId = (id) => document.getElementById(id);
  let soundCache = "";
  let cameraCache = "";
  let userCache = "";
  let toastTimer = 0;
  let uiReadySent = false;

  function announceUiReady() {
    if (uiReadySent) return;
    uiReadySent = true;
    const fontsReady = document.fonts?.ready || Promise.resolve();
    const imagesReady = [...document.images].map((image) => {
      if (image.complete) return Promise.resolve();
      return new Promise((resolve) => {
        image.addEventListener("load", resolve, { once: true });
        image.addEventListener("error", resolve, { once: true });
      });
    });
    const ready = Promise.all([fontsReady, ...imagesReady]).catch(() => undefined);
    const safetyTimeout = new Promise((resolve) => window.setTimeout(resolve, 8000));
    Promise.race([ready, safetyTimeout]).then(() => {
      window.setTimeout(() => send("ui-loaded"), 180);
    });
  }

  function activateTab(name, notify = true) {
    if (!tabs.includes(name)) return;
    document.querySelectorAll(".tab-button").forEach((button) => {
      button.classList.toggle("active", button.dataset.tab === name);
      button.setAttribute("aria-current", button.dataset.tab === name ? "page" : "false");
    });
    document.querySelectorAll(".tab-view").forEach((view) => {
      view.classList.toggle("active", view.dataset.view === name);
    });
    if (notify) send("tab", { value: tabs.indexOf(name) });
  }

  function setSelect(id, items, selected, emptyText) {
    const select = byId(id);
    const values = Array.isArray(items) ? items : [];
    const signature = values.join("\u0000");
    if (select.dataset.signature !== signature) {
      select.replaceChildren();
      if (!values.length) {
        const option = document.createElement("option");
        option.value = "-1";
        option.textContent = emptyText;
        select.append(option);
      } else {
        values.forEach((value, index) => {
          const option = document.createElement("option");
          option.value = String(index);
          option.textContent = value;
          select.append(option);
        });
      }
      select.dataset.signature = signature;
    }
    select.value = String(selected);
    if (select.selectedIndex < 0 && select.options.length) select.selectedIndex = 0;
  }

  function renderRows(containerId, items, selected, cacheName, emptyText, role) {
    const container = byId(containerId);
    const values = Array.isArray(items) ? items : [];
    const signature = values.join("\u0000");
    const cache = cacheName === "sounds" ? soundCache : cacheName === "cameras" ? cameraCache : userCache;
    if (cache !== signature) {
      container.replaceChildren();
      if (!values.length) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        empty.textContent = emptyText;
        container.append(empty);
      } else {
        values.forEach((title, index) => {
          const row = document.createElement("button");
          row.type = "button";
          row.className = "media-row";
          row.dataset.index = String(index);
          row.setAttribute("role", "option");
          const number = document.createElement("span");
          number.className = "row-index";
          number.textContent = String(index + 1).padStart(2, "0");
          const label = document.createElement("span");
          label.className = "row-title";
          label.textContent = title;
          row.append(number, label);
          container.append(row);
        });
      }
      if (cacheName === "sounds") soundCache = signature;
      else if (cacheName === "cameras") cameraCache = signature;
      else userCache = signature;
    }
    container.querySelectorAll(".media-row").forEach((row) => {
      const active = Number(row.dataset.index) === selected;
      row.classList.toggle("selected", active);
      row.setAttribute("aria-selected", active ? "true" : "false");
    });
    container.dataset.role = role;
  }

  function setStatusDot(id, active) {
    byId(id)?.classList.toggle("online", Boolean(active));
  }

  function updateState(state) {
    if (!state || state.type !== "state") return;

    const online = Boolean(state.zoomOnline);
    byId("zoom-status").classList.toggle("is-online", online);
    document.querySelector(".connection-card").classList.toggle("is-online", online);
    byId("zoom-status-title").textContent = online ? "Zoom.exe запущен" : "Zoom.exe не найден";
    byId("zoom-status-detail").textContent = online && state.zoomPid ? `PID ${state.zoomPid}` : "Состояние приложения";
    byId("zoom-card-title").textContent = online ? "Клиент запущен" : "Не обнаружен";
    byId("zoom-card-detail").textContent = online ? "Можно использовать инструменты" : "Запусти Zoom, чтобы продолжить";

    setSelect("microphone-select", state.microphones, state.selectedMic, "Устройства не найдены");
    setSelect("neon-audio-select", state.neonAudioDevices, state.selectedAudio, "Устройства не найдены");
    renderRows("sound-list", state.sounds, state.selectedSound, "sounds", "Добавь звуковые файлы, чтобы начать", "sound");
    renderRows("camera-list", state.cameras, state.selectedCamera, "cameras", "Сначала запусти сервис Neon", "camera");
    renderRows("users-list", state.users, -1, "users", "Нажми «Обновить», чтобы прочитать список", "user");

    byId("sound-count").textContent = state.sounds?.length ? `${state.sounds.length} файлов в библиотеке` : "Файлы не добавлены";
    byId("camera-count").textContent = state.cameras?.length ? `${state.cameras.length} камер` : "Нет созданных камер";
    byId("user-count").textContent = `${state.users?.length || 0} участников`;

    byId("volume-slider").min = String(state.volumeMin ?? 0);
    byId("volume-slider").max = String(state.volumeMax ?? 200);
    byId("volume-slider").value = String(state.volume ?? 100);
    byId("volume-value").textContent = `${state.volume ?? 100}%`;
    byId("bass-slider").value = String(state.bass ?? 0);
    byId("bass-value").textContent = `${state.bass ?? 0}%`;
    byId("route-phones").checked = Boolean(state.routePhones);
    byId("route-mic").checked = Boolean(state.routeMic);
    byId("auto-next-list").checked = Boolean(state.autoNextList);
    byId("auto-next-random").checked = Boolean(state.autoNextRandom);

    byId("cable-status").textContent = state.cableStatus || "Аудиоустройство не найдено";
    byId("now-playing").textContent = state.nowPlaying || "—";
    byId("playback-indicator").textContent = state.playing ? "Воспроизведение" : "Ожидание";

    byId("spam-status").textContent = state.spamStatus || "Готов.";
    byId("spam-state-label").textContent = state.spamRunning ? "Выполняется" : "Готов к запуску";
    setStatusDot("spam-live-dot", state.spamRunning);
    byId("hand-status").textContent = state.handStatus || "Готов.";

    byId("neon-status").textContent = state.neonStatus || "Neon: не запущен";
    byId("neon-state-label").textContent = state.neonAlive ? "Сервис активен" : "Сервис не запущен";
    setStatusDot("neon-live-dot", state.neonAlive);

    byId("users-status").textContent = state.usersStatus || "Готов.";
    if (state.initial) {
      byId("spam-text").value = state.spamText || "";
      byId("spam-interval").value = state.spamInterval || "2000";
      byId("spam-limit").value = state.spamLimit ?? "0";
    }
    if (state.settings) {
      byId("gain-min").value = String(state.settings.min ?? 0);
      byId("gain-max").value = String(state.settings.max ?? 200);
    }
  }

  function showToast(message) {
    const toast = byId("toast");
    toast.textContent = message;
    toast.classList.add("visible");
    window.clearTimeout(toastTimer);
    toastTimer = window.setTimeout(() => toast.classList.remove("visible"), 1800);
  }

  function runAction(action) {
    const actions = {
      "browse-sounds": "browse-sounds",
      "browse-folder": "browse-folder",
      play: "play",
      "play-random": "play-random",
      "spam-start": "spam-start",
      "spam-stop": "spam-stop",
      "hand-start": "hand-start",
      "hand-stop": "hand-stop",
      "neon-start": "neon-start",
      "neon-stop": "neon-stop",
      "neon-create": "neon-create",
      "neon-delete": "neon-delete",
      "neon-add-video": "neon-add-video",
      "neon-play": "neon-play",
      "neon-pause": "neon-pause",
      "neon-stop-camera": "neon-stop-camera",
      "users-refresh": "users-refresh",
      "users-clear": "users-clear"
    };
    if (action === "settings-close") {
      byId("settings-modal").classList.remove("active");
      byId("settings-modal").setAttribute("aria-hidden", "true");
      return;
    }
    if (actions[action]) send(actions[action]);
  }

  document.querySelectorAll(".tab-button").forEach((button) => {
    button.addEventListener("click", () => activateTab(button.dataset.tab));
  });

  document.body.addEventListener("click", (event) => {
    const button = event.target.closest("[data-action]");
    if (!button) return;
    runAction(button.dataset.action);
  });

  byId("join-form").addEventListener("submit", (event) => {
    event.preventDefault();
    send("join", { meeting: byId("meeting-id").value, password: byId("meeting-pass").value });
  });

  byId("sound-list").addEventListener("click", (event) => {
    const row = event.target.closest(".media-row");
    if (row) send("sound-select", { index: Number(row.dataset.index) });
  });
  byId("sound-list").addEventListener("dblclick", (event) => {
    if (event.target.closest(".media-row")) send("play");
  });
  byId("camera-list").addEventListener("click", (event) => {
    const row = event.target.closest(".media-row");
    if (row) send("camera-select", { index: Number(row.dataset.index) });
  });
  byId("users-list").addEventListener("click", (event) => {
    const row = event.target.closest(".media-row");
    if (row) send("user-select", { index: Number(row.dataset.index) });
  });

  byId("microphone-select").addEventListener("change", (event) => send("microphone", { index: Number(event.target.value) }));
  byId("neon-audio-select").addEventListener("change", (event) => send("neon-audio", { index: Number(event.target.value) }));
  byId("volume-slider").addEventListener("input", (event) => {
    byId("volume-value").textContent = `${event.target.value}%`;
    send("volume", { value: Number(event.target.value) });
  });
  byId("bass-slider").addEventListener("input", (event) => {
    byId("bass-value").textContent = `${event.target.value}%`;
    send("bass", { value: Number(event.target.value) });
  });

  ["route-phones", "route-mic", "auto-next-list", "auto-next-random"].forEach((id) => {
    byId(id).addEventListener("change", (event) => send("check", { name: id, checked: event.target.checked }));
  });

  byId("settings-open").addEventListener("click", () => {
    byId("settings-modal").classList.add("active");
    byId("settings-modal").setAttribute("aria-hidden", "false");
  });
  byId("settings-modal").addEventListener("click", (event) => {
    if (event.target === byId("settings-modal")) runAction("settings-close");
  });
  document.addEventListener("keydown", (event) => {
    if (event.key === "Escape") runAction("settings-close");
  });
  document.querySelector('[data-action="settings-save"]').addEventListener("click", () => {
    send("settings-save", { min: byId("gain-min").value, max: byId("gain-max").value });
    runAction("settings-close");
  });

  if (window.chrome?.webview) {
    window.chrome.webview.addEventListener("message", (event) => {
      const message = event.data;
      if (message?.type === "state") {
        updateState(message);
        if (message.initial) announceUiReady();
      }
      else if (message?.type === "toast") showToast(message.text || "");
    });
  }
  activateTab("join", false);
  send("ready");
})();
