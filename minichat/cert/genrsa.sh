openssl genrsa -out minichat_privkey.pem 2048

openssl rsa -in minichat_privkey.pem -out minichat_pubkey.pem -pubout

#openssl rsa -noout -text -in minichat_privkey.pem

