package `fun`.antioch.com.longboard

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.graphics.Color
import android.support.v7.app.AppCompatActivity
import android.util.Log
import android.widget.Toast
import java.io.InputStream
import java.io.OutputStream
import java.nio.charset.Charset
import java.util.*
import kotlin.collections.HashMap


enum class Data(val value: Char) {
    THROTTLE('t'),
    LIGHT_TYPE('l'),
    LIGHT_SPEED('s'),
    LIGHT_COLOR('c')
}

enum class Lighting(val value: Char) {
    LIGHT_OFF('o'),
    LIGHT_FLASH('f'),
    LIGHT_CHASE('c'),
    LIGHT_FADE('a'),
    LIGHT_SOLID('s')
}

object ArduinoBluetooth {
    // bluetooth communication constants
    private const val DELIMITER: Char = '\r'
    private const val STOP: Char = '\n'

    // bluetooth connection objects
    private var mOutputStream: OutputStream? = null
    private var mInputStream: InputStream? = null
    private var mSocket: BluetoothSocket? = null
    @Volatile private var mInputThreadRunning: Boolean = false

    // data to be sent to the arduino bluetooth
    private var throttle: String? = null
    var lightType: Lighting? = null
    var lightColor: Int? = null
    var lightSpeed: Int? = null

    // listeners to incoming bluetooth data
    val dataListeners: HashMap<String, (String)->Unit> = HashMap()

    // whether the arduino bluetooth is connected
    var isConnected: Boolean = false
        private set


    fun setThrottle(throttleString: String, context: Context) {
        var num: Int;
        num = try {
            throttleString.toInt()
        } catch (nfe: NumberFormatException) {
            -1
        }

        if (num > 180 || num < 0) {
            Toast.makeText(context, "Invalid throttle value", Toast.LENGTH_LONG)
        } else {
            throttle = throttleString
        }
    }

    private fun sendQueuedData() {
        throttle?.let {
            sendData("${Data.THROTTLE.value}$it$DELIMITER")
            throttle = null
        }
        lightSpeed?.let {
            sendData("${Data.LIGHT_SPEED.value}$it$DELIMITER")
            lightSpeed = null
        }
        lightColor?.let {
            val color: String = "${Color.red(it)},${Color.green(it)},${Color.blue(it)}"
            sendData("${Data.LIGHT_COLOR.value}$color$DELIMITER")
            lightColor = null
        }
        lightType?.let {
            sendData("${Data.LIGHT_TYPE.value}${it.value}$DELIMITER")
            lightType = null
        }
        endDataSend()
    }

    private fun sendData(data: String) {
        if (!isConnected) {
            return
        }
        if (data.contains("$STOP")) {
            throw IllegalArgumentException("Data to send cannot contains stop symbol \"$STOP\"")
        }
        val toSend = "$data"
        Log.d("tosend", toSend)
        mOutputStream?.write(toSend.toByteArray())
        mOutputStream?.flush()
    }

    private fun endDataSend() {
        mOutputStream?.write(STOP.toInt())
        mOutputStream?.flush()
    }

    fun disconnect() {
        if (!isConnected) return
        isConnected = false
        mInputThreadRunning = false
        mInputStream?.close()
        mOutputStream?.close()
        mSocket?.close()
    }

    fun connect(activity: AppCompatActivity, repeat: Int): Boolean {
        if (isConnected) return true
        if (repeat == -1) return false

        // attempt connection
        val device = getDevice(activity)
        isConnected = device != null && connectToDevice(device)

        if (isConnected) {
            // handshake
            mOutputStream?.write(1)
            mOutputStream?.flush()
            // maintain connection
            listenForData()
            return true
        }

        // possible try again
        return connect(activity, repeat - 1)
    }

    private fun getDevice(activity: AppCompatActivity) : BluetoothDevice? {
        val bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        if (!bluetoothAdapter.isEnabled) {
            val enableBluetooth = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            activity.startActivityForResult(enableBluetooth, 0)
        }

        val devices = bluetoothAdapter.bondedDevices
        if (devices.size > 0) {
            for (device in devices) {
                if (device.address == MODEM_ADDRESS) {
                    return device
                }
            }
        }

        return null
    }

    private fun connectToDevice(device: BluetoothDevice) : Boolean {
        try {
            //Standard SerialPortService ID
            val uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb")
            mSocket = device.createRfcommSocketToServiceRecord(uuid)
            mSocket?.connect()
            mOutputStream = mSocket?.outputStream
            mInputStream = mSocket?.inputStream
        } catch (e: Exception) {
            e.printStackTrace()
            return false
        }
        return true
    }

    private fun listenForData() {
        mInputThreadRunning = true

        dataListeners["sendOk"] = {
            if (it == "OK") {
                sendQueuedData()
            }
        }

        Thread {
            val buffer = ByteArray(1024) {0}
            var bufferPos = 0

            while (!Thread.currentThread().isInterrupted && mInputThreadRunning) {
                val toRead = mInputStream?.available() ?: 0
                if (toRead <= 0) continue
                val data = ByteArray(toRead) {0}
                mInputStream?.read(data)

                for (i in 0 until data.size) {
                    if (data[i] == DELIMITER.toByte()) {
                        val toConvert = ByteArray(bufferPos, {j -> buffer[j]})
                        val charset = Charset.forName("US-ASCII")
                        val msg = String(toConvert, charset)
                        bufferPos = 0
                        for (i in dataListeners) {
                            i.value(msg)
                        }
                    } else {
                        buffer[bufferPos++] = data[i]
                    }
                }


            }
        }.start()
    }
}