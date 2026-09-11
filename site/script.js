(function () {
  const cards = Array.from(document.querySelectorAll('.help-card'));
  const search = document.querySelector('#command-search');
  const empty = document.querySelector('#empty-state');
  const filters = Array.from(document.querySelectorAll('.filter-button'));
  const sections = ['release', 'coverage', 'help', 'install']
    .map(function (id) { return document.getElementById(id); })
    .filter(Boolean);
  const navLinks = Array.from(document.querySelectorAll('.site-nav a'));
  let activeFilter = 'all';

  function updateCommands() {
    const term = (search && search.value ? search.value : '').trim().toLowerCase();
    let visible = 0;
    cards.forEach(function (card) {
      const haystack = (card.dataset.command + ' ' + card.textContent).toLowerCase();
      const matchesTerm = !term || haystack.indexOf(term) !== -1;
      const matchesCategory = activeFilter === 'all' || card.dataset.category === activeFilter;
      const visibleCard = matchesTerm && matchesCategory;
      card.classList.toggle('is-hidden', !visibleCard);
      if (visibleCard) visible += 1;
    });
    if (empty) empty.hidden = visible !== 0;
  }

  if (search) search.addEventListener('input', updateCommands);
  filters.forEach(function (button) {
    button.addEventListener('click', function () {
      activeFilter = button.dataset.filter || 'all';
      filters.forEach(function (item) { item.classList.toggle('is-active', item === button); });
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
      window.setTimeout(function () { if (label) label.textContent = original; }, 1200);
    });
  });

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
