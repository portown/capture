FROM portown/alpine-mingw-w64:latest

RUN mkdir -p /home/capture
WORKDIR /home/capture

ENV ZLIB_VERSION 1.2.11
ENV ZLIB_DOWNLOAD_URL http://www.zlib.net/zlib-$ZLIB_VERSION.tar.gz
ENV ZLIB_DOWNLOAD_SIG http://www.zlib.net/zlib-$ZLIB_VERSION.tar.gz.asc
ENV ZLIB_KEY 783FCD8E58BCAFBA
ENV ZLIB_ROOT /usr/local/x86_64-w64-mingw32

ENV LIBPNG_VERSION 1.6.8
ENV LIBPNG_DOWNLOAD_URL https://sourceforge.net/projects/libpng/files/libpng16/older-releases/$LIBPNG_VERSION/libpng-$LIBPNG_VERSION.tar.gz/download
ENV LIBPNG_DOWNLOAD_SHA1 a6d0be6facada6b4f26c24ffb23eaa2da8df9bd9
ENV LIBPNG_ROOT /usr/local/x86_64-w64-mingw32

ENV BOOST_VERSION 1.63.0
ENV BOOST_DOWNLOAD_SHA1 9f1dd4fa364a3e3156a77dc17aa562ef06404ff6
ENV BOOST_ROOT /usr/local/x86_64-w64-mingw32

RUN apk add --no-cache \
    curl \
    gnupg \
    make \
 # Download zlib
 && curl -fsSL "$ZLIB_DOWNLOAD_URL" -o zlib.tar.gz \
 && curl -fsSL "$ZLIB_DOWNLOAD_SIG" -o zlib.tar.gz.asc \
 && gpg --batch --keyserver pgp.mit.edu `if test "x$http_proxy" != "x"; then echo "--keyserver-options http-proxy=$http_proxy"; fi` --recv-keys "$ZLIB_KEY" \
 && gpg --batch --verify zlib.tar.gz.asc zlib.tar.gz \
 && gpg --batch --yes --delete-keys "$ZLIB_KEY" && rm -Rf /root/.gnupg \
 && rm -f zlib.tar.gz.asc \
 # Download libpng
 && curl -fsSL "$LIBPNG_DOWNLOAD_URL" -o libpng.tar.gz \
 && echo "$LIBPNG_DOWNLOAD_SHA1  libpng.tar.gz" | sha1sum -c - \
 # Download Boost
 && curl -fsSL "https://sourceforge.net/projects/boost/files/boost/$BOOST_VERSION/boost_`echo $BOOST_VERSION | sed -e 's/\./_/g'`.tar.bz2/download" -o boost.tar.bz2 \
 && echo "$BOOST_DOWNLOAD_SHA1  boost.tar.bz2" | sha1sum -c - \
 # Download Boost/expected
 && curl -fsSL https://github.com/ptal/expected/archive/master.zip -o boost_expected.zip \
 # Cleanup
 && apk del --no-cache --purge \
    curl \
    gnupg \
 # Build zlib
 && tar -xzf zlib.tar.gz \
 && ( cd zlib-$ZLIB_VERSION && CC=x86_64-w64-mingw32-gcc AR=x86_64-w64-mingw32-ar RANLIB=x86_64-w64-mingw32-ranlib ./configure --prefix=$ZLIB_ROOT --static --64 && make && make install && cd .. ) \
 && rm -Rf zlib-$ZLIB_VERSION/ zlib.tar.gz \
 # Build libpng
 && tar -xzf libpng.tar.gz \
 && ( cd libpng-$LIBPNG_VERSION && CPPFLAGS="$CPPFLAGS -I$ZLIB_ROOT/include" LDFLAGS="$LDFLAGS -L$ZLIB_ROOT/lib" ./configure \
        --prefix=$LIBPNG_ROOT --host=x86_64-w64-mingw32 --disable-shared --enable-static \
    && make && make install \
    && cd .. ) \
 && rm -Rf libpng-$LIBPNG_VERSION/ libpng.tar.gz \
 # Build Boost
 && apk add --no-cache \
    build-base \
 && tar -xjf boost.tar.bz2 \
 && ( cd boost_`echo $BOOST_VERSION | sed -e 's/\./_/g'` \
    && echo "using gcc : mingw : x86_64-w64-mingw32-g++ ;" > user-config.jam \
    && ./bootstrap.sh --without-icu --prefix=$BOOST_ROOT --with-libraries=container,filesystem,system \
    && ./b2 --user-config=user-config.jam \
        --layout=system \
        toolset=gcc-mingw \
        target-os=windows \
        variant=release \
        link=static \
        runtime-link=shared \
        threading=multi \
        threadapi=win32 \
        abi=ms \
        architecture=x86 \
        binary-format=pe \
        address-model=64 \
        install \
    && cd .. ) \
 && rm -Rf boost_`echo $BOOST_VERSION | sed -e 's/\./_/g'`/ boost.tar.bz2 \
 && apk del --no-cache --purge \
    build-base \
 # Build Boost/expected
 && unzip -q boost_expected.zip \
 && cp -R expected-master/include/boost $BOOST_ROOT/include \
 && rm -Rf expected-master/ boost_expected.zip
