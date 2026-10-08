document.querySelectorAll("[data-sourcemeta-ui-dropdown]").forEach((dropdown) => {
  const toggle = dropdown.querySelector("[data-sourcemeta-ui-dropdown-toggle]");
  const menu = dropdown.querySelector("[data-sourcemeta-ui-dropdown-menu]");

  function close() {
    // Whoever was reading the menu would be left standing on something that
    // is no longer there, with no way back to the control that opened it
    const wasInside = dropdown.contains(document.activeElement);
    menu.classList.remove("show");
    toggle.setAttribute("aria-expanded", "false");
    if (wasInside) {
      toggle.focus();
    }
  }

  toggle.addEventListener("click", () => {
    if (toggle.getAttribute("aria-expanded") === "true") {
      close();
    } else {
      menu.classList.add("show");
      toggle.setAttribute("aria-expanded", "true");
    }
  });

  document.addEventListener("click", (event) => {
    if (!dropdown.contains(event.target)) {
      close();
    }
  });

  dropdown.addEventListener("focusout", (event) => {
    if (!dropdown.contains(event.relatedTarget)) {
      close();
    }
  });

  document.addEventListener("keydown", (event) => {
    if (event.key === "Escape") {
      close();
    }
  });
});
