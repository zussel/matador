(function () {
    const output = document.getElementById('output');
    document.getElementById('login').onclick = function () {
        const credentials = {
            username: document.getElementById('uname').value,
            password: document.getElementById('pwd').value
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
    };
})();
