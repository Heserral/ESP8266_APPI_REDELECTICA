# ESP8266_APPI_REDELECTICA

Lo primero que necesitas es obtener un token para la api de esios/red electrica. Es muy fácil. sólo tienes que enviar un mail a   consultasios@ree.es con el texto del asunto asunto  **Solicitud de token personal** . En 10 min responden con el token

Cambiar en Arduino:

const char *ssid     = "XXXXX";

const char *password = "XXXXX";

const String token = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

# API ESIOS RED ELECTICA

https://api.esios.ree.es

https://api.esios.ree.es/indicators //Devuelve los códiigos de funciones
10391 es el código para la tarifa 2.0TD

## Obtener datos diarios de la tarifa
https://api.esios.ree.es/indicators/10391   //2.0 TD

Authorization: Token token=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

Accept: application/json; application/vnd.esios-api-v2+json

## Obtener datos en un rango de fechas y localización
https://api.esios.ree.es/indicators/10391?start_date=2021-06-03T03%3A00%3A00Z&end_date=2021-06-03T04%3A00%3A00Z&geo_ids[]=8741

Authorization: Token token=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

Accept: application/json; application/vnd.esios-api-v2+json
