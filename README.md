# base64

This lib manage convertion of data to base64 encoded array, orignals data can be stored in file or in `uint8_t array`.

you can see an exemple of use on [Github](https://github.com/ox223252/img2b64), to convert imgs to base64 files.

```C
if ( encodeBase64 ( F2F, "in.ext", "out.b64", 0 ) )
{ // error case
}
```

```C
uint8_t buffer[40] = {...};
uint32_t size = 40;
if ( encodeBase64 ( S2F, &buffer, "out.b64", &size ) )
{ // error case
}
```

```C
uint8_t *buffer = NULL;
uint32_t size = 0;
if ( encodeBase64 ( F2S, "in.ext", &buffer, &size ) )
{ // error case
}
```

```C
uint8_t inBuf[40] = {...};
uint8_t *outBuf = NULL;
uint32_t size = 40;
if ( encodeBase64 ( S2S, "in.ext", &buffer, &size ) )
{ // error case
}
// now size = 40 / 3 * 4 + ( 40 % 3 )? 1 : 0;
//          = 56
```