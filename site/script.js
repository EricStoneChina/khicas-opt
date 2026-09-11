document.querySelectorAll('a[href^="#"]').forEach(function(link){link.addEventListener('click',function(){document.body.classList.add('navigated')})});
