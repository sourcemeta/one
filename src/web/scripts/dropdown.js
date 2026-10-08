document.querySelectorAll("[data-sourcemeta-ui-dropdown]").forEach((dropdown) => {
  const toggle = dropdown.querySelector("[data-sourcemeta-ui-dropdown-toggle]");
  const menu = dropdown.querySelector("[data-sourcemeta-ui-dropdown-menu]");

  function close() {
    menu.classList.remove("show");
    toggle.setAttribute("aria-expanded", "false");
  }

  // Whoever was reading the menu would be left standing on something that is
  // no longer there, with no way back to the control that opened it
  function dismiss() {
    const wasInside = dropdown.contains(document.activeElement);
    close();
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

  // Focus landing nowhere is focus that never went anywhere, such as a click
  // on a part of the menu that cannot hold it
  dropdown.addEventListener("focusout", (event) => {
    if (event.relatedTarget && !dropdown.contains(event.relatedTarget)) {
      close();
    }
  });

  document.addEventListener("keydown", (event) => {
    if (event.key === "Escape") {
      dismiss();
    }
  });
});
