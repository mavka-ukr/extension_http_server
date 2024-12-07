# Розширення http для Мавки

Це прототип.

```
bash збудувати.sh
мавка приклад_http.м
```

```shell
curl localhost:8080
```

```shell
wrk -t8 -c200 -d5s http://127.0.0.1:8080
```