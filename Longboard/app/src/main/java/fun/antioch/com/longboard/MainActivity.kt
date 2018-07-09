package `fun`.antioch.com.longboard

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.content.Intent
import android.graphics.Color
import android.graphics.PorterDuff

class MainActivity : AppCompatActivity() {
    lateinit var mConnectButton: Button

    lateinit var mOffButton: Button
    lateinit var mFlashButton: Button
    lateinit var mChaseButton: Button
    lateinit var mFadeButton: Button
    lateinit var mSolidButton: Button
    val options: Array<Button?> = Array<Button?>(5, {i -> null})
    val optionSelectColor: Int = Color.parseColor("#AAAAAA")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        mConnectButton = findViewById<Button>(R.id.start_button);

        mConnectButton.setOnClickListener {
            val i = Intent(this@MainActivity, MotorControlActivity::class.java)
            startActivity(i)
        }

        mOffButton = findViewById(R.id.off_option)
        mFlashButton = findViewById(R.id.flash_option)
        mChaseButton = findViewById(R.id.chase_option)
        mFadeButton = findViewById(R.id.fade_option)
        mSolidButton = findViewById(R.id.solid_option)
        options[0] = mOffButton
        options[1] = mFlashButton
        options[2] = mChaseButton
        options[3] = mFadeButton
        options[4] = mSolidButton

        for (i in options) {
            i?.setOnClickListener { view ->
                ArduinoBluetooth.lightType = when(i) {
                    mOffButton -> Lighting.LIGHT_OFF
                    mFlashButton -> Lighting.LIGHT_FLASH
                    mChaseButton -> Lighting.LIGHT_CHASE
                    mFadeButton -> Lighting.LIGHT_FADE
                    mSolidButton -> Lighting.LIGHT_SOLID
                    else -> Lighting.LIGHT_OFF
                }

                for (j in options) {
                    j?.background?.clearColorFilter();
                }
                i?.background.setColorFilter(optionSelectColor, PorterDuff.Mode.MULTIPLY)
                if (i != mOffButton) {
                    val intent = Intent(this@MainActivity, LightOptions::class.java)
                    startActivity(intent)
                }
            }
        }

    }
}