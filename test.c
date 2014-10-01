#include <stdio.h>

#define DEBUG_ON
#include "debug.h"

// #include "jsonparser.h"


char* json_ok = "{\"cod\":\"200\",\"message\":0.0044,\"city\":{\"id\":3080165,\"name\":\"Zielona Gora\",\"coord\":{\"lon\":15.50643,\"lat\":51.935478},\"country\":\"PL\",\"population\":0},\"cnt\":5,\"list\":[{\"dt\":1411898400,\"temp\":{\"day\":10.49,\"min\":7.42,\"max\":10.49,\"night\":7.42,\"eve\":10.49,\"morn\":10.49},\"pressure\":1030.82,\"humidity\":81,\"weather\":[{\"id\":801,\"main\":\"Clouds\",\"description\":\"few clouds\",\"icon\":\"02n\"}],\"speed\":2.51,\"deg\":212,\"clouds\":20},{\"dt\":1411984800,\"temp\":{\"day\":14.88,\"min\":7.39,\"max\":19.32,\"night\":12.81,\"eve\":19.32,\"morn\":7.39},\"pressure\":1029.75,\"humidity\":76,\"weather\":[{\"id\":802,\"main\":\"Clouds\",\"description\":\"scattered clouds\",\"icon\":\"03d\"}],\"speed\":3.41,\"deg\":228,\"clouds\":44},{\"dt\":1412071200,\"temp\":{\"day\":17.7,\"min\":11.54,\"max\":20.24,\"night\":15.11,\"eve\":19.35,\"morn\":11.54},\"pressure\":1029.36,\"humidity\":79,\"weather\":[{\"id\":501,\"main\":\"Rain\",\"description\":\"moderate rain\",\"icon\":\"10d\"}],\"speed\":2.67,\"deg\":268,\"clouds\":68,\"rain\":10},{\"dt\":1412157600,\"temp\":{\"day\":17.88,\"min\":12.99,\"max\":17.88,\"night\":12.99,\"eve\":15.64,\"morn\":15.43},\"pressure\":1025.38,\"humidity\":0,\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"speed\":4.22,\"deg\":259,\"clouds\":86,\"rain\":2.13},{\"dt\":1412244000,\"temp\":{\"day\":16.06,\"min\":9.24,\"max\":16.06,\"night\":9.24,\"eve\":13.36,\"morn\":13.71},\"pressure\":1028.64,\"humidity\":0,\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"speed\":5.11,\"deg\":310,\"clouds\":35,\"rain\":0.97}]}";
char* example = "{\"name\":\"test\", \"num\":5, \"array\":[1, 2 ], \"object\":{\"type\":\"some type\", \"value\":\"some value\"}, \"array_of_object\":[{\"name\":\"john\", \"age\":20 }, {\"name\":\"anna\", \"age\":21 } ], \"object_with_array\":{\"team\":[\"adam\", \"tom\", \"moni\"], \"strangers\":[\"kathy\", \"rolf\"]}}";
char* string_ok = "\"string\"";

int main(void) {

  LOG_ERROR("%s", __FILE__);

}
