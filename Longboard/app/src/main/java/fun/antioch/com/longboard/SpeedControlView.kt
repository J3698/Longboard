package `fun`.antioch.com.longboard

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.os.Vibrator
import android.support.v4.content.ContextCompat.getSystemService
import android.text.TextPaint
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View

/**
 * TODO: document your custom view class.
 */
class SpeedControlView : View {
    private var _exampleDimension: Float = 0f // TODO: use a default from R.dimen...

    var message: String
        get() = _message
        set(value) {
            _message = value
            invalidateTextPaintAndMeasurements()
            invalidate()
        }

    private var _message: String = resources.getString(R.string.speed_control_connect_msg)


    private var textPaint: TextPaint? = null
    private var textWidth: Float = 0f
    private var textHeight: Float = 0f
    private var padding: Float = 5f

    private val linePaint: Paint = Paint()

    private var triggerPulled: Boolean = false
    private var triggerY0: Float = 0f
    private var triggerY1: Float = 0f

    var maxHeight: Float = 0.75f
    var resolution: Int = 175
    var onThrottleListener: (throttle: Int) -> Unit = {}

    private var lastThrottle: Int = 0
    fun getLastThrottle() : Int = lastThrottle


    /**
     * In the example view, this dimension is the font size.
     */
    var exampleDimension: Float
        get() = _exampleDimension
        set(value) {
            _exampleDimension = value
            invalidateTextPaintAndMeasurements()
        }

    constructor(context: Context) : super(context) {
        init(null, 0)
    }

    constructor(context: Context, attrs: AttributeSet) : super(context, attrs) {
        init(attrs, 0)
    }

    constructor(context: Context, attrs: AttributeSet, defStyle: Int) : super(context, attrs, defStyle) {
        init(attrs, defStyle)
    }

    private fun init(attrs: AttributeSet?, defStyle: Int) {
        // Load attributes
        val a = context.obtainStyledAttributes(
                attrs, R.styleable.SpeedControlView, defStyle, 0)

        // Use getDimensionPixelSize or getDimensionPixelOffset when dealing with
        // values that should fall on pixel boundaries.
        _exampleDimension = a.getDimension(
                R.styleable.SpeedControlView_exampleDimension,
                exampleDimension)

        message = a.getString(R.styleable.SpeedControlView_message) ?: _message

        a.recycle()

        linePaint.strokeWidth = 3f
        linePaint.style = Paint.Style.FILL;
        linePaint.isAntiAlias = true;

        // Set up a default TextPaint object
        textPaint = TextPaint().apply {
            flags = Paint.ANTI_ALIAS_FLAG
            textAlign = Paint.Align.LEFT
            color = Color.BLACK
        }

        addEventHandlers()

        // Update TextPaint and text measurements from attributes
        invalidateTextPaintAndMeasurements()
    }

    private fun invalidateTextPaintAndMeasurements() {
        textPaint?.let {
            it.textSize = exampleDimension
            textWidth = it.measureText(_message)
            textHeight = it.fontMetrics.bottom
        }
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)

        canvas.drawText(_message, (width - textWidth) / 2, (height + textHeight) / 2, textPaint)

        linePaint.color = Color.BLACK
        canvas.drawLine(0f, height.toFloat(), width.toFloat(), height.toFloat(), linePaint)
        if (triggerPulled) {
            linePaint.strokeWidth = 0f
            if (triggerY1 < triggerY0) {
                linePaint.color = Color.GREEN
            } else {
                linePaint.color = Color.RED
            }
            canvas.drawRect(0f, triggerY0, width.toFloat(), triggerY1, linePaint)
        }
    }

    @SuppressLint("ClickableViewAccessibility")
    private fun addEventHandlers() {
        this.setOnTouchListener { _, event ->
            var throttle: Int = resolution / 2
            when (event.action) {
                // start drag event
                MotionEvent.ACTION_DOWN -> {
                    triggerPulled = true
                    triggerY0 = event.y
                    triggerY1 = event.y
                }
                // stop drag event
                MotionEvent.ACTION_UP -> {
                    triggerPulled = false
                }
                // drag event
                MotionEvent.ACTION_MOVE -> {
                    val rawDiff: Float = event.y - triggerY0
                    val maxPixels: Float = maxHeight * height
                    // cap maximum drag
                    triggerY1 = when {
                        rawDiff > maxPixels    -> triggerY0 + maxPixels
                        rawDiff < -maxPixels   -> triggerY0 - maxPixels
                        else                -> event.y
                    }
                    // keep drag on screen
                    triggerY1 = when {
                        triggerY1 > height - padding    -> height - padding
                        triggerY1 <  padding            -> padding
                        else                            -> triggerY1
                    }
                    // calculate throttle
                    val diff: Float = triggerY0 - triggerY1
                    throttle = (resolution * (maxPixels + diff) / (2 * maxPixels)).toInt()
                }
            }

            if (throttle != lastThrottle) {
                onThrottleListener(throttle)
                lastThrottle = throttle
                invalidate()
            }

            true
        }
    }
}
