# Capture

[![Build Status](https://travis-ci.org/portown/capture.svg?branch=master)](https://travis-ci.org/portown/capture)

## 概要

本アプリは、Windows 上で動作する、範囲指定可能な画面キャプチャツールです。


## 使用方法

アプリを起動した状態でキャプチャ開始キーを押下することでキャプチャモードになります。

キャプチャモード中に、キャプチャしたい範囲をドラッグすることでその範囲をキャプチャできます。

キャプチャした画像はタブ管理されており、複数の画像を保存しないまま保持しておくことができます。

対応している保存形式は BMP, PNG です。

なお、キャプチャ開始キーは自由に設定することができます。
このとき、Ctrl や Alt といった修飾キーと組み合わせることも可能です。


## 依存ライブラリ

* libpng
* zlib
* Boost.Exception
* Boost.Optional
