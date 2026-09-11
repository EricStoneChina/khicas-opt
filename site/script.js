/**
 * KhiCAS Website Interactive Features
 */

(function() {
  'use strict';

  // Smooth scroll for anchor links
  function initSmoothScroll() {
    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
      anchor.addEventListener('click', function(e) {
        const href = this.getAttribute('href');
        if (href === '#') return;
        
        e.preventDefault();
        const target = document.querySelector(href);
        if (target) {
          const offset = 80; // navbar height
          const targetPosition = target.getBoundingClientRect().top + window.pageYOffset - offset;
          window.scrollTo({
            top: targetPosition,
            behavior: 'smooth'
          });
        }
      });
    });
  }

  // Mobile menu toggle
  function initMobileMenu() {
    const menuToggle = document.querySelector('.menu-toggle');
    const navMenu = document.querySelector('.nav-menu');
    
    if (!menuToggle || !navMenu) return;
    
    menuToggle.addEventListener('click', function() {
      const isOpen = navMenu.classList.toggle('active');
      this.classList.toggle('active');
      this.setAttribute('aria-expanded', isOpen);
      
      // Prevent scroll when menu is open
      document.body.style.overflow = isOpen ? 'hidden' : '';
    });
    
    // Close menu when clicking outside
    document.addEventListener('click', function(e) {
      if (!menuToggle.contains(e.target) && !navMenu.contains(e.target)) {
        navMenu.classList.remove('active');
        menuToggle.classList.remove('active');
        menuToggle.setAttribute('aria-expanded', 'false');
        document.body.style.overflow = '';
      }
    });
    
    // Close menu when clicking a link
    navMenu.querySelectorAll('a').forEach(link => {
      link.addEventListener('click', function() {
        navMenu.classList.remove('active');
        menuToggle.classList.remove('active');
        menuToggle.setAttribute('aria-expanded', 'false');
        document.body.style.overflow = '';
      });
    });
  }

  // Navbar scroll effect
  function initNavbarScroll() {
    const navbar = document.querySelector('.navbar');
    if (!navbar) return;
    
    let lastScroll = 0;
    
    window.addEventListener('scroll', function() {
      const currentScroll = window.pageYOffset;
      
      if (currentScroll > 100) {
        navbar.classList.add('scrolled');
      } else {
        navbar.classList.remove('scrolled');
      }
      
      lastScroll = currentScroll;
    });
  }

  // Animate elements on scroll
  function initScrollAnimations() {
    const observerOptions = {
      threshold: 0.1,
      rootMargin: '0px 0px -50px 0px'
    };
    
    const observer = new IntersectionObserver(function(entries) {
      entries.forEach(entry => {
        if (entry.isIntersecting) {
          entry.target.classList.add('animate-in');
        }
      });
    }, observerOptions);
    
    // Observe feature cards, steps, and doc cards
    document.querySelectorAll('.feature-card, .step, .doc-card').forEach(el => {
      el.classList.add('animate-on-scroll');
      observer.observe(el);
    });
  }

  // Terminal typing effect
  function initTerminalAnimation() {
    const terminal = document.querySelector('.terminal-body');
    if (!terminal) return;
    
    const lines = terminal.querySelectorAll('.terminal-line, .terminal-output');
    
    // Hide all lines initially
    lines.forEach(line => {
      line.style.opacity = '0';
    });
    
    // Animate lines in sequence
    let delay = 0;
    lines.forEach((line, index) => {
      setTimeout(() => {
        line.style.transition = 'opacity 0.3s ease';
        line.style.opacity = '1';
      }, delay);
      
      // Longer delay for output lines
      if (line.classList.contains('terminal-output')) {
        delay += 800;
      } else {
        delay += 400;
      }
    });
  }

  // Copy code to clipboard (for future use)
  function initCodeCopy() {
    document.querySelectorAll('code').forEach(code => {
      if (code.parentElement.tagName === 'PRE') return;
      
      code.addEventListener('click', function() {
        const text = this.textContent;
        navigator.clipboard.writeText(text).then(() => {
          // Show temporary tooltip
          const tooltip = document.createElement('span');
          tooltip.textContent = 'Copied!';
          tooltip.style.cssText = `
            position: absolute;
            background: #10b981;
            color: white;
            padding: 4px 8px;
            border-radius: 4px;
            font-size: 12px;
            z-index: 1000;
            animation: fadeOut 2s forwards;
          `;
          this.style.position = 'relative';
          this.appendChild(tooltip);
          
          setTimeout(() => tooltip.remove(), 2000);
        });
      });
    });
  }

  // Track download button clicks (analytics placeholder)
  function initAnalytics() {
    document.querySelectorAll('a[href*="releases"]').forEach(link => {
      link.addEventListener('click', function() {
        // Placeholder for analytics tracking
        console.log('Download link clicked:', this.href);
      });
    });
  }

  // Add external link icons
  function initExternalLinks() {
    document.querySelectorAll('a[target="_blank"]').forEach(link => {
      if (!link.querySelector('svg')) {
        link.setAttribute('rel', 'noopener noreferrer');
      }
    });
  }

  // Keyboard navigation improvements
  function initKeyboardNav() {
    // Focus visible on keyboard navigation
    document.addEventListener('keydown', function(e) {
      if (e.key === 'Tab') {
        document.body.classList.add('keyboard-nav');
      }
    });
    
    document.addEventListener('mousedown', function() {
      document.body.classList.remove('keyboard-nav');
    });
  }

  // Add animation CSS
  function addAnimationStyles() {
    const style = document.createElement('style');
    style.textContent = `
      .animate-on-scroll {
        opacity: 0;
        transform: translateY(20px);
        transition: opacity 0.6s ease, transform 0.6s ease;
      }
      
      .animate-on-scroll.animate-in {
        opacity: 1;
        transform: translateY(0);
      }
      
      .navbar.scrolled {
        box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
      }
      
      .nav-menu.active {
        display: flex;
        flex-direction: column;
        position: fixed;
        top: 64px;
        left: 0;
        right: 0;
        background: white;
        padding: 2rem;
        box-shadow: 0 8px 16px rgba(0, 0, 0, 0.1);
        z-index: 999;
      }
      
      .menu-toggle.active span:nth-child(1) {
        transform: rotate(45deg) translate(5px, 5px);
      }
      
      .menu-toggle.active span:nth-child(2) {
        opacity: 0;
      }
      
      .menu-toggle.active span:nth-child(3) {
        transform: rotate(-45deg) translate(6px, -6px);
      }
      
      body.keyboard-nav *:focus {
        outline: 2px solid #2563eb;
        outline-offset: 2px;
      }
      
      @keyframes fadeOut {
        0% { opacity: 1; }
        100% { opacity: 0; }
      }
    `;
    document.head.appendChild(style);
  }

  // Initialize all features when DOM is ready
  function init() {
    addAnimationStyles();
    initSmoothScroll();
    initMobileMenu();
    initNavbarScroll();
    initScrollAnimations();
    initTerminalAnimation();
    initCodeCopy();
    initAnalytics();
    initExternalLinks();
    initKeyboardNav();
    
    console.log('KhiCAS website initialized');
  }

  // Run on DOM ready
  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
  } else {
    init();
  }

  // Reload animations on page show (back/forward navigation)
  window.addEventListener('pageshow', function(event) {
    if (event.persisted) {
      initScrollAnimations();
      initTerminalAnimation();
    }
  });

})();
