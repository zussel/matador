$(document).ready(function() {
    $.getJSON("https://api.github.com/repos/zussel/matador/releases/latest", function(data) {
        $('#release-version').append('<h3 class="centered" >Latest release: ' + data.tag_name + '</h3>');
    })
});
