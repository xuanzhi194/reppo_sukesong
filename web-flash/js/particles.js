/**
 * Particle network background — cyan/white tech aesthetic
 */
(function () {
  const canvas = document.getElementById('particle-canvas');
  if (!canvas) return;

  const ctx = canvas.getContext('2d');
  let width = 0;
  let height = 0;
  let particles = [];
  let mouse = { x: -9999, y: -9999, active: false };
  let animationId = 0;

  const COLORS = ['#00d9ff', '#ffffff', '#7ee8ff'];
  const LINK_DIST = 120;
  const MOUSE_RADIUS = 140;
  const MOUSE_FORCE = 0.08;

  function particleCount() {
    const area = width * height;
    return Math.min(2200, Math.max(800, Math.floor(area / 420)));
  }

  function resize() {
    width = canvas.width = window.innerWidth;
    height = canvas.height = window.innerHeight;
    initParticles();
  }

  function initParticles() {
    const n = particleCount();
    particles = [];
    for (let i = 0; i < n; i++) {
      particles.push({
        x: Math.random() * width,
        y: Math.random() * height,
        vx: (Math.random() - 0.5) * 0.35,
        vy: (Math.random() - 0.5) * 0.35,
        r: Math.random() * 1.4 + 0.4,
        color: COLORS[Math.floor(Math.random() * COLORS.length)],
        alpha: Math.random() * 0.45 + 0.35,
      });
    }
  }

  function onMouseMove(e) {
    mouse.x = e.clientX;
    mouse.y = e.clientY;
    mouse.active = true;
  }

  function onMouseLeave() {
    mouse.active = false;
  }

  function tick() {
    ctx.clearRect(0, 0, width, height);

    for (const p of particles) {
      if (mouse.active) {
        const dx = mouse.x - p.x;
        const dy = mouse.y - p.y;
        const dist = Math.hypot(dx, dy);
        if (dist < MOUSE_RADIUS && dist > 0) {
          const f = (1 - dist / MOUSE_RADIUS) * MOUSE_FORCE;
          p.vx += (dx / dist) * f;
          p.vy += (dy / dist) * f;
        }
      }

      p.x += p.vx;
      p.y += p.vy;
      p.vx *= 0.995;
      p.vy *= 0.995;

      if (p.x < 0) { p.x = 0; p.vx *= -1; }
      if (p.x > width) { p.x = width; p.vx *= -1; }
      if (p.y < 0) { p.y = 0; p.vy *= -1; }
      if (p.y > height) { p.y = height; p.vy *= -1; }

      ctx.beginPath();
      ctx.arc(p.x, p.y, p.r, 0, Math.PI * 2);
      ctx.fillStyle = p.color;
      ctx.globalAlpha = p.alpha;
      ctx.fill();
    }

    ctx.globalAlpha = 1;
    for (let i = 0; i < particles.length; i++) {
      for (let j = i + 1; j < particles.length; j++) {
        const a = particles[i];
        const b = particles[j];
        const dx = a.x - b.x;
        const dy = a.y - b.y;
        const dist = Math.hypot(dx, dy);
        if (dist < LINK_DIST) {
          const alpha = (1 - dist / LINK_DIST) * 0.22;
          ctx.strokeStyle = `rgba(0, 217, 255, ${alpha})`;
          ctx.lineWidth = 0.6;
          ctx.beginPath();
          ctx.moveTo(a.x, a.y);
          ctx.lineTo(b.x, b.y);
          ctx.stroke();
        }
      }
    }

    animationId = requestAnimationFrame(tick);
  }

  window.addEventListener('resize', resize);
  window.addEventListener('mousemove', onMouseMove);
  window.addEventListener('mouseleave', onMouseLeave);

  resize();
  tick();

  window.addEventListener('beforeunload', () => cancelAnimationFrame(animationId));
})();
