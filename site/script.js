(function () {
  const cards = Array.from(document.querySelectorAll('.command-card'));
  const search = document.querySelector('#command-search');
  const empty = document.querySelector('#empty-state');
  const filters = Array.from(document.querySelectorAll('.filter-button'));
  let activeFilter = 'all';

  function updateCommands() {
    const term = (search && search.value ? search.value : '').trim().toLowerCase();
    let visible = 0;

    cards.forEach(function (card) {
      const text = (card.dataset.command + ' ' + card.textContent).toLowerCase();
      const languages = card.dataset.language || '';
      const matchesTerm = !term || text.indexOf(term) !== -1;
      const matchesFilter = activeFilter === 'all' || languages.indexOf(activeFilter) !== -1;
      const isVisible = matchesTerm && matchesFilter;
      card.classList.toggle('is-hidden', !isVisible);
      if (isVisible) visible += 1;
    });

    if (empty) empty.hidden = visible !== 0;
  }

  if (search) search.addEventListener('input', updateCommands);

  filters.forEach(function (button) {
    button.addEventListener('click', function () {
      activeFilter = button.dataset.filter || 'all';
      filters.forEach(function (item) {
        item.classList.toggle('is-active', item === button);
      });
      updateCommands();
    });
  });

  document.querySelectorAll('.copy-command').forEach(function (button) {
    button.addEventListener('click', async function () {
      const value = button.dataset.copy || '';
      try {
        await navigator.clipboard.writeText(value);
      } catch (_) {
        const area = document.createElement('textarea');
        area.value = value;
        area.setAttribute('readonly', '');
        area.style.position = 'fixed';
        area.style.opacity = '0';
        document.body.appendChild(area);
        area.select();
        document.execCommand('copy');
        area.remove();
      }

      const label = button.firstChild;
      const original = label ? label.textContent : '';
      if (label) label.textContent = '已复制 ';
      window.setTimeout(function () {
        if (label) label.textContent = original;
      }, 1200);
    });
  });

  const sections = ['packages', 'capabilities', 'commands', 'install']
    .map(function (id) { return document.getElementById(id); })
    .filter(Boolean);
  const navLinks = Array.from(document.querySelectorAll('.main-nav a'));

  if ('IntersectionObserver' in window && sections.length) {
    const observer = new IntersectionObserver(function (entries) {
      entries.forEach(function (entry) {
        if (!entry.isIntersecting) return;
        navLinks.forEach(function (link) {
          link.classList.toggle('is-current', link.getAttribute('href') === '#' + entry.target.id);
        });
      });
    }, { rootMargin: '-35% 0px -55% 0px', threshold: 0 });
    sections.forEach(function (section) { observer.observe(section); });
  }

  updateCommands();
}());
