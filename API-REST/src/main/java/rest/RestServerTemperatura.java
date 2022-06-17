package rest;


import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import clases.VCalidadAgua;
import clases.VTemperatura;
import io.netty.handler.codec.mqtt.MqttQoS;
import io.vertx.core.AbstractVerticle;
import io.vertx.core.Promise;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.core.buffer.Buffer;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import io.vertx.mqtt.MqttClient;
import io.vertx.mqtt.MqttClientOptions;
import io.vertx.mysqlclient.MySQLConnectOptions;
import io.vertx.mysqlclient.MySQLPool;
import io.vertx.sqlclient.PoolOptions;
import io.vertx.sqlclient.Row;
import io.vertx.sqlclient.RowSet;
import io.vertx.sqlclient.Tuple;

public class RestServerTemperatura extends AbstractVerticle {

	private Gson gson;
	private MqttClient mqttClient;
	MySQLPool mySqlClient;
	
	
	public void start(Promise<Void> startFuture) {
		gson = new Gson();
		

		// Instantiating a Gson serialize object using specific date format
		gson = new GsonBuilder().setDateFormat("yyyy-MM-dd").create();

		// Defining the router object
		Router router = Router.router(vertx);

		// Handling any server startup result
		vertx.createHttpServer().requestHandler(router::handle).listen(8080, result -> {
			if (result.succeeded()) {
				startFuture.complete();
			} else {
				startFuture.fail(result.cause());
			}
		});
		

		// Defining URI paths for each method in RESTful interface, including body
		
		router.route("/api/*").handler(BodyHandler.create());
		router.route("/api/alltemperatura*").handler(this::getAll);
		router.get("/api/temperatura/:id").handler(this::getTemperaturaSensorX);
		router.post("/api/addsensorTemp").handler(this::postsensortemp);
		router.post("/api/addsensorCad").handler(this::postsensoragua);
		
		 mqttClient = MqttClient.create(getVertx(),new MqttClientOptions().setAutoKeepAlive(true));
		 mqttClient.connect(1883, "192.168.103.186", s -> {});
		 
		
		
		
		MySQLConnectOptions connectOptions = new MySQLConnectOptions().setPort(3306).setHost("localhost")
				.setDatabase("acuariodb").setUser("root").setPassword("root");
		
		PoolOptions poolOptions = new PoolOptions().setMaxSize(5);

		mySqlClient = MySQLPool.pool(vertx, connectOptions, poolOptions);
		
	}
	
	
	
	private void getAll(RoutingContext routingContext) {
		mySqlClient.query("SELECT * FROM temperatura;", res -> {
			if (res.succeeded()) {
				// Get the result set
				RowSet<Row> resultSet = res.result();
				System.out.println(resultSet.size());
				JsonArray result = new JsonArray();
				for (Row elem : resultSet) {
					result.add(JsonObject.mapFrom(new VTemperatura(elem.getInteger("id"), elem.getInteger("idSensor"),
							elem.getDouble("temperatura"),elem.getLong("Mtemporal"))));
				}
				System.out.println(result.toString());
			} else {
				System.out.println("Error: " + res.cause().getLocalizedMessage());
			}
		});
	}
	
	private void getTemperaturaSensorX(RoutingContext routingContext) {
		
		Integer id=Integer.parseInt(routingContext.request().getParam("id"));
		
		mySqlClient.getConnection(connection -> {
			if (connection.succeeded()) {
				connection.result().preparedQuery("SELECT * FROM temperatura WHERE idSensor =? ",
						Tuple.of(id), res -> {
							if (res.succeeded()) {
								// Get the result set
								RowSet<Row> resultSet = res.result();
								System.out.println(resultSet.size());
								List<VTemperatura> result = new ArrayList<VTemperatura>();
								for (Row elem : resultSet) {
									result.add(new VTemperatura(elem.getInteger("idtemperatura"), elem.getInteger("idSensor"),
											elem.getDouble("temperaturadat"),elem.getLong("mTemporal")));
								}
								System.out.println(gson.toJson(result));
							} else {
								System.out.println("Error: " + res.cause().getLocalizedMessage());
							}
							connection.result().close();
						});
			} else {
				System.out.println(connection.cause().toString());
			}
		});
	}
	
	
	
	private void postsensortemp(RoutingContext routingContext){
		VTemperatura sensortemp = gson.fromJson(routingContext.getBodyAsString(), VTemperatura.class);	
		sensortemp.setMtemporal(Calendar.getInstance().getTimeInMillis()/1000);
		
		mySqlClient.preparedQuery("INSERT INTO temperatura ( idSensor, temperaturadat, mTemporal) VALUES (?,?,?)",
				
				Tuple.of( sensortemp.getidSensor(),sensortemp.getTemperatura(),sensortemp.getMtemporal()),handler -> {
					
				if (handler.succeeded()) {
					routingContext.response().setStatusCode(201).putHeader("content-type", "application/json; charset=utf-8")
					.end(gson.toJson(sensortemp));
					
					if(sensortemp.getTemperatura()>=30.0) {
						
					mqttClient.publish("topic_1", Buffer.buffer("1"), MqttQoS.AT_LEAST_ONCE, false, false);

					}else {
					mqttClient.publish("topic_1", Buffer.buffer("0"), MqttQoS.AT_LEAST_ONCE, false, false);
					}
					
				}else {
					routingContext.response().setStatusCode(401).putHeader("content-type", "application/json; charset=utf-8")
					.end((JsonObject.mapFrom(handler.cause()).encodePrettily()));
					System.out.println("Error"+handler.cause().getLocalizedMessage());
				}
			});
		
	}
	
	private void postsensoragua(RoutingContext routingContext){
		VCalidadAgua sensortemp = gson.fromJson(routingContext.getBodyAsString(), VCalidadAgua.class);	
		sensortemp.setMtemporalcad(Calendar.getInstance().getTimeInMillis()/1000);
		
		mySqlClient.preparedQuery("INSERT INTO calidadagua ( idSensorcad, calidadAgua, Mtemporalcad) VALUES (?,?,?)",
				
				Tuple.of( sensortemp.getIdSensorcad(),sensortemp.getCalidadAgua(),sensortemp.getMtemporalcad()),handler -> {
					
				if (handler.succeeded()) {
					routingContext.response().setStatusCode(201).putHeader("content-type", "application/json; charset=utf-8")
					.end(gson.toJson(sensortemp));
					
					if(sensortemp.getCalidadAgua()>=100.0) {
						
					mqttClient.publish("topic_2", Buffer.buffer("1"), MqttQoS.AT_LEAST_ONCE, false, false);

					}else {
					mqttClient.publish("topic_2", Buffer.buffer("0"), MqttQoS.AT_LEAST_ONCE, false, false);
					}
					
				}else {
					routingContext.response().setStatusCode(401).putHeader("content-type", "application/json; charset=utf-8")
					.end((JsonObject.mapFrom(handler.cause()).encodePrettily()));
					System.out.println("Error"+handler.cause().getLocalizedMessage());
				}
			});
		
	}
	
	

}
