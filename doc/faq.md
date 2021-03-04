# Frequently Asked Questions

## 1. no matching function for call to 'HX711::HX711(const int&, const int&)'

I'm getting the following error:

```exit status 1 no matching function for call to 'HX711::HX711(const int&, const int&)'```

The new interface is that the begin(...) method will obtain the pin parameters. So instead of using:
```
HX711 scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
```
Use:
```
HX711 scale;
```
And then in ```void setup() { ... }``` initialize the pins like this:
```
scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
```
Please refer to this example for more details:
https://github.com/bogde/HX711/blob/master/examples/HX711_basic_example/HX711_basic_example.ino#L7-L12
