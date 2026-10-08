document.querySelectorAll("[data-sourcemeta-ui-dropdown]").forEach((dropdown) => {
  const toggle = dropdown.querySelector("[data-sourcemeta-ui-dropdown-toggle]");
  const menu = dropdown.querySelector("[data-sourcemeta-ui-dropdown-menu]");

  function close() {
    menu.classList.remove("show");
    toggle.setAttribute("aria-expanded", "false");
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

  document.addEventListener("keydown", (event) => {
    if (event.key === "Escape") {
      close();
    }
  });
});
