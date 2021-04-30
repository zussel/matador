$(document).ready(function() {
    let modal = $('.modal');
    $('#submit').click(() => {
        const credentials = {
            username: $('input[name=username]').val(),
            password: $('input[name=password]').val()
        };

        axios.post('/api/v1/auth/login', credentials)
            .then(function (res) {
                console.log(res);
                window.location = "secure"
//                output.className = 'container';
//                output.innerHTML = res.data;
            })
            .catch(function (err) {
                console.error(err);
//                output.className = 'container text-danger';
//                output.innerHTML = err.message;
            });
    });
    $('#cancel').click(() => {
        modal.hide();
    });
    $('#login').click(() => {
        modal.show();
    });
});
