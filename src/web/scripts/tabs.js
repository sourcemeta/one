document.querySelectorAll("[data-sourcemeta-ui-tab-group]").forEach((group) => {
  const tabButtons = group.querySelectorAll("[data-sourcemeta-ui-tab-target]");
  const tabViews = group.querySelectorAll("[data-sourcemeta-ui-tab-id]");
  const urlParam = group.getAttribute("data-sourcemeta-ui-tab-url-param");

  function selectTab(tab, name, updateURL) {
    tabButtons.forEach(element => element.classList.remove("active"));
    tab.classList.add("active");
    tabViews.forEach((panel) => {
      if (panel.getAttribute("data-sourcemeta-ui-tab-id") === name) {
        panel.classList.remove("d-none");
      } else {
        panel.classList.add("d-none");
      }
    });

    if (updateURL && urlParam) {
      const url = new URL(window.location);
      url.searchParams.set(urlParam, name);
      window.history.replaceState({}, "", url);
    }
  }

  if (urlParam) {
    const initialTab = new URL(window.location).searchParams.get(urlParam);
    let matched = false;
    if (initialTab) {
      tabButtons.forEach((element) => {
        if (element.getAttribute("data-sourcemeta-ui-tab-target") === initialTab) {
          selectTab(element, initialTab, true);
          matched = true;
        }
      });
    }

    if (!matched && tabButtons[0]) {
      selectTab(tabButtons[0],
        tabButtons[0].getAttribute("data-sourcemeta-ui-tab-target"),
        false);
    }
  } else if (tabButtons[0]) {
    selectTab(tabButtons[0],
      tabButtons[0].getAttribute("data-sourcemeta-ui-tab-target"),
      false);
  }

  tabButtons.forEach((tab) => {
    tab.addEventListener("click", () => {
      const targetId = tab.getAttribute("data-sourcemeta-ui-tab-target");
      selectTab(tab, targetId, true);
    });
  });
});
