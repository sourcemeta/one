const to = new URLSearchParams(window.location.search).get("to");
if (to) {
  document.querySelectorAll("[data-sourcemeta-ui-login]").forEach((link) => {
    link.setAttribute("href",
      link.getAttribute("href") + "?to=" + encodeURIComponent(to));
  });
}
