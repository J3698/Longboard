package `fun`.antioch.com.longboard

import android.content.SharedPreferences
import android.graphics.Color
import android.graphics.PorterDuff
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.preference.PreferenceManager
import android.widget.Button
import android.widget.SeekBar

import com.pes.androidmaterialcolorpickerdialog.ColorPicker;

class LightOptions : AppCompatActivity() {
    private lateinit var preferences: SharedPreferences

    private lateinit var mBackButton: Button
    private lateinit var mColorButton: Button
    private lateinit var mSpeedSeeker: SeekBar
    private lateinit var picker: ColorPicker

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_light_options)

        mColorButton = findViewById(R.id.color_change_button)
        mSpeedSeeker = findViewById(R.id.light_speed_control)
        mBackButton = findViewById(R.id.light_back_button)

        preferences = PreferenceManager.getDefaultSharedPreferences(this)
        color = color

        picker = ColorPicker(this@LightOptions,
                Color.red(color),
                Color.green(color),
                Color.blue(color))

        picker.setCallback {newColor ->
            color = newColor
            val r: Int = Color.red(color) / 3
            val g: Int = Color.green(color) / 3
            val b: Int = Color.blue(color) / 3
            ArduinoBluetooth.lightColor = Color.rgb(r, g, b)
            picker.dismiss()
        }

        mColorButton.setOnClickListener {
            picker.show()
        }

        mBackButton.setOnClickListener {
            finish()
        }

        mSpeedSeeker.setOnSeekBarChangeListener(SeekListener())
    }

    class SeekListener: SeekBar.OnSeekBarChangeListener {
        override fun onStartTrackingTouch(p0: SeekBar?) {}
        override fun onStopTrackingTouch(p0: SeekBar?) {}
        override fun onProgressChanged(p0: SeekBar?, p1: Int, p2: Boolean) {
            ArduinoBluetooth.lightSpeed = p1
        }
    }

    private var color: Int
        get() {
            return preferences.getInt("color", Color.BLACK)
        }
        set(value) {
            val editor: SharedPreferences.Editor = preferences.edit()
            editor.putInt("color", value)
            editor.apply()
            val avg = (Color.red(color) + Color.green(color) + Color.blue(color)) / 3
            mColorButton.setTextColor(when {
                avg < 126    -> Color.WHITE
                else         -> Color.BLACK
            })
            mColorButton.background.setColorFilter(color, PorterDuff.Mode.SRC)
            mColorButton.invalidate()
        }
}