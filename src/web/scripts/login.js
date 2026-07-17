const params = new URLSearchParams(window.location.search);
const to = params.get("to");
const policies = params.getAll("policy");
document.querySelectorAll("[data-sourcemeta-ui-login]").forEach((link) => {
  if (policies.length > 0 &&
      !policies.includes(link.getAttribute("data-sourcemeta-ui-login"))) {
    link.remove();
    return;
  }
  if (to) {
    link.setAttribute("href",
      link.getAttribute("href") + "?to=" + encodeURIComponent(to));
  }
});
