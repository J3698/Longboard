package `fun`.antioch.com.longboard

import android.support.v7.app.AppCompatActivity

import android.os.Bundle
import android.os.Handler
import android.util.Log

import android.widget.Button


const val MODEM_ADDRESS = "00:06:66:D8:19:3C"

const val CONNECT_ATTEMPTS: Int = 3

class MotorControlActivity : AppCompatActivity() {
    lateinit var mConnectButton: Button
    lateinit var mBackButton: Button
    lateinit var mSpeedController: SpeedControlView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_motor_control)

        mConnectButton = findViewById(R.id.dc_button)
        mBackButton = findViewById(R.id.motor_back_button)
        mSpeedController = findViewById(R.id.speed_controller)

        mBackButton.setOnClickListener {
            finish()
        }
        mConnectButton.setOnClickListener {
            if (ArduinoBluetooth.isConnected) {
                finish()
            } else {
                connect()
            }
        }
        connect()
        mSpeedController.onThrottleListener = {
            ArduinoBluetooth.setThrottle("$it", MotorControlActivity@this)
        }
    }

    private fun connect() {
        mConnectButton.isEnabled = false
        mBackButton.isEnabled = false
        mSpeedController.message = resources.getString(R.string.speed_control_connect_msg)
        val handler = Handler()
        Thread {
            Log.d("info", "About to connect")
            ArduinoBluetooth.connect(MotorControlActivity@this, CONNECT_ATTEMPTS)
            Log.d("info", "Connected")
            handler.post {
                if (ArduinoBluetooth.isConnected) {
                    mConnectButton.text = resources.getText(R.string.motor_main_button_connected)
                    mSpeedController.message = resources.getString(R.string.speed_control_go_msg)
                    mBackButton.visibility = Button.GONE
                } else {
                    mConnectButton.text = resources.getText(R.string.motor_main_button_failed)
                    mSpeedController.message = resources.getString(R.string.speed_control_fail_msg)
                    mBackButton.visibility = Button.VISIBLE
                    mBackButton.isEnabled = true
                }
                mConnectButton.isEnabled = true
            }
        }.start()
    }


}
