// $('#markdown-toc').affix({
//   offset: {
//     top: 100,
//     bottom: function () {
//       return (this.bottom = $('.footer').outerHeight(true))
//     }
//   }
// });
$(':header').each(function() {
    var id = $(this).attr('id');
    $(this).removeAttr('id');
    $(this).prepend("<a class=\"anchor\" id=\"" + id + "\"></a>");
});
// $(':header').prepend("<a class=\"anchor\"></a>");
$('#markdown-toc ul').addClass('nav nav-stacked');
$('body').scrollspy({ target: '.bs-docs-sidebar', offset: 80 });