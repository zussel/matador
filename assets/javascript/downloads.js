$(document).ready(function() {
    $.getJSON("https://api.github.com/repos/zussel/matador/releases/latest", function(data) {
        $('#zips').append(
            '<a href="https://github.com/zussel/matador/archive/' + data.tag_name + '.zip" class="btn btn-success">zip</button>' +
            '<a href="https://github.com/zussel/matador/archive/' + data.tag_name + '.tar.gz" class="btn btn-success">tar.gz</button>');
        data.assets.forEach(function(asset) {
            console.log(asset);
            $('#downloads > tbody:last-child').append('<tr><td><a href="' + asset.browser_download_url + '">' + asset.name + '</a></td><td>' + asset.size + '</td></tr>');
        });
    })
});
