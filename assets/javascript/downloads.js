$(document).ready(function() {
    $.getJSON("https://api.github.com/repos/zussel/oos/releases/latest", function(data) {
        data.assets.forEach(function(asset) {
            console.log(asset);
            $('#downloads > tbody:last-child').append('<tr><td><a href="' + asset.browser_download_url + '">' + asset.name + '</a></td><td>' + asset.size + '</td></tr>');
        });
    })
});
