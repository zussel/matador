// $('#markdown-toc').affix({
//   offset: {
//     top: 100,
//     bottom: function () {
//       return (this.bottom = $('.footer').outerHeight(true))
//     }
//   }
// });
$('#markdown-toc ul').addClass('nav nav-stacked');
$('body').scrollspy({ target: '.bs-docs-sidebar', offset: 80 });