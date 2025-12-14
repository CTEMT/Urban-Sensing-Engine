package it.cnr.istc.ctemt.uspe;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.google.firebase.messaging.FirebaseMessaging;
import com.google.gson.Gson;
import com.google.gson.JsonObject;

import java.util.concurrent.Executors;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationManagerCompat;

import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class MainActivity extends Activity {

    private static final String TAG = "MainActivity";
    private static final Gson gson = new Gson();
    private final OkHttpClient client = new OkHttpClient.Builder()
            .connectTimeout(30, java.util.concurrent.TimeUnit.SECONDS)
            .readTimeout(30, java.util.concurrent.TimeUnit.SECONDS)
            .writeTimeout(30, java.util.concurrent.TimeUnit.SECONDS)
            .build();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        NotificationChannel channel = new NotificationChannel("default_channel", "Default Channel",
                NotificationManager.IMPORTANCE_HIGH);
        channel.setDescription("Channel for FCM notifications");

        NotificationManager manager = getSystemService(NotificationManager.class);
        manager.createNotificationChannel(channel);

        if (!NotificationManagerCompat.from(this).areNotificationsEnabled()) {
            // Show dialog to user
            new AlertDialog.Builder(this)
                    .setTitle("Enable Notifications")
                    .setMessage(
                            "Notifications are disabled. Please enable them in settings to receive important updates.")
                    .setPositiveButton("Open Settings", (dialog, which) -> {
                        Intent intent = new Intent();
                        intent.setAction(Settings.ACTION_APP_NOTIFICATION_SETTINGS);
                        intent.putExtra(Settings.EXTRA_APP_PACKAGE, getPackageName());
                        startActivity(intent);
                    })
                    .setNegativeButton("Cancel", null)
                    .show();
        }

        checkID();
    }

    private void checkID() {
        String id = getSharedPreferences("uspe", MODE_PRIVATE).getString("item_id", null);
        if (id == null) {
            Log.d(TAG, "No ID found, retrieving from server...");
            Executors.newSingleThreadExecutor().execute(() -> {
                final Request.Builder builder = new Request.Builder()
                        .url("https://10.0.2.2:8443/items?type=User&name=Riccardo").get();
                try (Response response = client.newCall(builder.build()).execute()) {
                    if (response.isSuccessful()) {
                        JsonObject responseBody = gson.fromJson(response.body().charStream(), JsonObject.class);
                        String item_id = responseBody.getAsJsonPrimitive("id").getAsString();
                        getSharedPreferences("uspe", MODE_PRIVATE).edit()
                                .putString("item_id", item_id).apply();
                    } else
                        Log.e(TAG, "Failed to retrieve item: " + response.code());
                } catch (Exception e) {
                    Log.e(TAG, "Error retrieving item", e);
                }
            });
        } else {
            Log.d(TAG, "ID found in SharedPreferences: " + id);
            checkFCMToken(id);
        }
    }

    private void checkFCMToken(@NonNull String item_id) {
        FirebaseMessaging.getInstance().getToken().addOnCompleteListener(task -> {
            if (task.isSuccessful()) {
                Log.d(TAG, "FCM token retrieved successfully");
                String fcm_token = task.getResult();
                Log.d(TAG, "FCM Token: " + fcm_token);
                Executors.newSingleThreadExecutor().execute(() -> newFCMToken(item_id, fcm_token));
            } else
                Log.e("Connection", "Failed to get FCM token", task.getException());
        });
    }

    private void newFCMToken(@NonNull String item_id, @NonNull String fcm_token) {
        final Request.Builder builder = new Request.Builder().url("https://10.0.2.2:8443/fcm_tokens")
                .post(RequestBody.create("{\"id\": \"" + item_id + "\", \"token\": \"" + fcm_token + "\"}",
                        MediaType.parse("application/json")));
        try (Response response = client.newCall(builder.build()).execute()) {
            if (response.isSuccessful()) {
                String responseBody = response.body().string();
                Log.d(TAG, "Token registered successfully: " + responseBody);
            } else
                Log.e(TAG, "Failed to register token: " + response.code());
        } catch (Exception e) {
            Log.e(TAG, "Error during token registration", e);
        }
    }
}
