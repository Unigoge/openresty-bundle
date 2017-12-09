#!/bin/bash
export BUILD_LOG=$PWD/build.log
apt-mark unhold nginx-common
apt-mark unhold nginx-extras
apt-get -y update 2>&1 | tee $BUILD_LOG
apt-get -y install nginx-extras build-essential libpcre3-dev libssl-dev libgeoip-dev libpq-dev libxslt1-dev libgd2-xpm-dev 2>&1 | tee -a $BUILD_LOG
cd /home/openresty/service_status/v2.0/config/openresty-1.11.2.2
./configure \
--sbin-path=/usr/sbin/nginx \
--conf-path=/etc/nginx/nginx.conf \
--error-log-path=/var/log/nginx/error.log \
--http-client-body-temp-path=/var/lib/nginx/body \
--http-fastcgi-temp-path=/var/lib/nginx/fastcgi \
--http-log-path=/var/log/nginx/access.log \
--http-proxy-temp-path=/var/lib/nginx/proxy \
--http-scgi-temp-path=/var/lib/nginx/scgi \
--http-uwsgi-temp-path=/var/lib/nginx/uwsgi \
--lock-path=/var/lock/nginx.lock \
--pid-path=/var/run/nginx.pid \
--with-sha1=/usr/include/openssl \
--with-md5=/usr/include/openssl \
--with-http_stub_status_module \
--with-http_secure_link_module \
--with-luajit \
--with-pcre-jit \
--with-debug \
--with-stream \
--with-stream_ssl_module \
--with-http_auth_request_module \
--with-http_addition_module \
--with-http_gunzip_module \
--with-http_image_filter_module \
--with-http_dav_module \
--with-http_flv_module \
--with-http_geoip_module \
--with-http_gzip_static_module \
--with-http_realip_module \
--with-http_stub_status_module \
--with-http_ssl_module \
--with-http_sub_module \
--with-http_xslt_module \
--with-ipv6 \
--with-http_postgres_module \
--add-module=bundle/nginx-sticky-module-ng \
--add-module=bundle/stream-lua-nginx-module \
--add-module=bundle/ngx_lua_ipc 2>&1 | tee -a $BUILD_LOG
make 2>&1 | tee -a $BUILD_LOG
make install 2>&1 | tee -a $BUILD_LOG
cd -
if [ -f nginx.conf ]; then
    if [ -f /etc/nginx/nginx.conf ]; then
	mv /etc/nginx/nginx.conf /etc/nginx/nginx.conf.original
	cp nginx.conf /etc/nginx/nginx.conf
	diff /etc/nginx/nginx.conf /etc/nginx/nginx.conf.original 2>&1 | tee -a $BUILD_LOG
    else
	cp nginx.conf /etc/nginx/nginx.conf
    fi
fi

apt-get -y autoclean
apt-get -y autoremove
apt-mark hold nginx-common
apt-mark hold nginx-extras
