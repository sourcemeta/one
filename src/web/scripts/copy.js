document.querySelectorAll("[data-sourcemeta-ui-copy]").forEach((button) => {
  button.addEventListener("click", () => {
    const text = button.getAttribute("data-sourcemeta-ui-copy");
    navigator.clipboard.writeText(text);
    const icon = button.querySelector("i");
    if (icon) {
      icon.classList.replace("bi-clipboard", "bi-check");
      setTimeout(() => icon.classList.replace("bi-check", "bi-clipboard"), 1500);
    }
  });
});
