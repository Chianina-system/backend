
# linux/certs
./backend_tuning /usr/local/szw/oocimpl/cacheD/in/linux/certs/configuration.txt \
/usr/local/szw/oocimpl/cacheD/in/linux/certs/entry.txt \
/usr/local/szw/oocimpl/cacheD/in/linux/certs/new-final.txt \
/usr/local/szw/oocimpl/cacheD/in/linux/certs/new-nodes.txt \
/usr/local/szw/oocimpl/cacheD/in/linux/certs/id_stmt_info.txt 0 1 1 1 0 1 0 0 0 \
> ./cache_result/certs_res.txt 2>&1

# drivers/sfi
./backend_tuning /usr/local/szw/oocimpl/cacheD/in/linuxdrivers/sfi/configuration.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/sfi/entry.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/sfi/new-final.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/sfi/new-nodes.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/sfi/id_stmt_info.txt 0 1 1 1 0 1 0 0 0 \
> ./cache_result/sfi_res.txt 2>&1

# drivers/macintosh
./backend_tuning /usr/local/szw/oocimpl/cacheD/in/linuxdrivers/macintosh/configuration.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/macintosh/entry.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/macintosh/new-final.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/macintosh/new-nodes.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/macintosh/id_stmt_info.txt 0 1 1 1 0 1 0 0 0 \
> ./cache_result/macintosh_res.txt 2>&1

# drivers/clocksource
./backend_tuning /usr/local/szw/oocimpl/cacheD/in/linuxdrivers/clocksource/configuration.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/clocksource/entry.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/clocksource/new-final.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/clocksource/new-nodes.txt \
/usr/local/szw/oocimpl/cacheD/in/linuxdrivers/clocksource/id_stmt_info.txt 0 1 1 1 0 1 0 0 0 \
> ./cache_result/clocksource_res.txt 2>&1

# openssl/fuzz
./backend_tuning /usr/local/szw/oocimpl/cacheD/in/openssl/fuzz/configuration.txt \
/usr/local/szw/oocimpl/cacheD/in/openssl/fuzz/entry.txt \
/usr/local/szw/oocimpl/cacheD/in/openssl/fuzz/new-final.txt \
/usr/local/szw/oocimpl/cacheD/in/openssl/fuzz/new-nodes.txt \
/usr/local/szw/oocimpl/cacheD/in/openssl/fuzz/id_stmt_info.txt 0 1 1 1 0 1 0 0 0 \
> ./cache_result/fuzz_res.txt 2>&1

# openssl/engine
./backend_tuning /usr/local/szw/oocimpl/cacheD/in/openssl/engine/configuration.txt \
/usr/local/szw/oocimpl/cacheD/in/openssl/engine/entry.txt \
/usr/local/szw/oocimpl/cacheD/in/openssl/engine/new-final.txt \
/usr/local/szw/oocimpl/cacheD/in/openssl/engine/new-nodes.txt \
/usr/local/szw/oocimpl/cacheD/in/openssl/engine/id_stmt_info.txt 0 1 1 1 0 1 0 0 0 \
> ./cache_result/engine_res.txt 2>&1
