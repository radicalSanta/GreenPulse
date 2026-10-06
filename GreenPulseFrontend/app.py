import time
import requests
import streamlit as st
import pandas as pd
import pydeck as pdk

# ============================================================
# GreenPulse Streamlit Dashboard
# ============================================================

API_BASE_URL = "https://yours-toolbox-mil-revision.trycloudflare.com"
REFRESH_SECONDS = 5

st.set_page_config(
    page_title="GreenPulse",
    page_icon="🌿",
    layout="wide",
    initial_sidebar_state="collapsed",
)

# ------------------------------------------------------------
# Styling
# ------------------------------------------------------------

st.markdown(
    """
    <style>
        .block-container {
            padding-top: 1.5rem;
            padding-bottom: 2rem;
            max-width: 1450px;
        }

        .gp-title {
            font-size: 2.7rem;
            font-weight: 800;
            letter-spacing: -0.04em;
            margin-bottom: 0;
        }

        .gp-subtitle {
            color: #6b7280;
            font-size: 1rem;
            margin-top: 0.1rem;
            margin-bottom: 1.5rem;
        }

        .score-card {
            border: 1px solid rgba(128,128,128,.25);
            border-radius: 16px;
            padding: 18px;
            min-height: 145px;
        }

        .score-label {
            font-size: .88rem;
            color: #6b7280;
            font-weight: 600;
        }

        .score-value {
            font-size: 2.25rem;
            font-weight: 800;
            margin-top: 8px;
        }

        .score-unit {
            font-size: .9rem;
            color: #6b7280;
        }

        .alert-box {
            border-radius: 14px;
            padding: 16px 18px;
            border: 1px solid rgba(128,128,128,.25);
        }

        .section-title {
            font-size: 1.35rem;
            font-weight: 750;
            margin-top: 1.2rem;
            margin-bottom: .6rem;
        }
    </style>
    """,
    unsafe_allow_html=True,
)

# ------------------------------------------------------------
# Helpers
# ------------------------------------------------------------

def get_json(path: str):
    response = requests.get(
        f"{API_BASE_URL}{path}",
        timeout=20,
    )
    response.raise_for_status()
    return response.json()


def score_color(score):
    if score >= 80:
        return "green"
    if score >= 60:
        return "orange"
    if score >= 40:
        return "orange"
    return "red"


def alert_color(level):
    return {
        "NORMAL": "green",
        "WATCH": "orange",
        "WARNING": "red",
        "CRITICAL": "red",
    }.get(level, "gray")


def score_card(title, score):
    score = float(score)
    st.markdown(
        f"""
        <div class="score-card">
            <div class="score-label">{title}</div>
            <div class="score-value" style="color:{score_color(score)}">
                {score:.1f}
            </div>
            <div class="score-unit">/ 100</div>
        </div>
        """,
        unsafe_allow_html=True,
    )


# ------------------------------------------------------------
# Load dashboard state
# ------------------------------------------------------------

try:
    dashboard = get_json("/dashboard/current")
    latest = get_json("/latest")
    history = get_json("/history")
except requests.RequestException as exc:
    st.error(
        "GreenPulse backend is unavailable. "
        "Start FastAPI with: uvicorn main:app --reload"
    )
    st.caption(str(exc))
    st.stop()

ems = dashboard["ems"]
local = dashboard["local_telemetry"]
environment = dashboard["environmental_context"]

# ------------------------------------------------------------
# Header
# ------------------------------------------------------------

st.markdown('<div class="gp-title">GreenPulse</div>', unsafe_allow_html=True)
st.markdown(
    '<div class="gp-subtitle">Environmental Monitoring System</div>',
    unsafe_allow_html=True,
)

# ------------------------------------------------------------
# Hero: EMS + three dimensions
# ------------------------------------------------------------

hero_left, hero_right = st.columns([1, 2.2])

with hero_left:
    st.metric("Environmental Monitoring Score", f"{ems['score']:.1f} / 100")

    alert = ems["alert_level"]
    st.markdown(
        f"""
        <div class="alert-box">
            <strong>Alert: {alert}</strong><br>
            Primary driver: {ems["primary_driver"].replace("_", " ").title()}
        </div>
        """,
        unsafe_allow_html=True,
    )

with hero_right:
    c1, c2, c3 = st.columns(3)

    with c1:
        score_card("Ground Stability", ems["ground_stability"])

    with c2:
        score_card("Human Pressure", ems["human_pressure"])

    with c3:
        score_card("Environmental Quality", ems["environmental_quality"])

# ------------------------------------------------------------
# Advisory
# ------------------------------------------------------------

st.markdown('<div class="section-title">Alerts & Advisory</div>', unsafe_allow_html=True)

st.info(ems["advisory"])

# ------------------------------------------------------------
# Current local telemetry
# Deliberately minimal: raw temperature/humidity + selected
# environmental measurements. Internal EMS scores remain hidden.
# ------------------------------------------------------------

st.markdown('<div class="section-title">Current Conditions</div>', unsafe_allow_html=True)

t1, t2, t3, t4, t5, t6 = st.columns(6)

with t1:
    st.metric("Temperature", f"{local['temperature_c']:.1f} °C")

with t2:
    st.metric("Humidity", f"{local['humidity_percent']:.0f} %")

with t3:
    st.metric("Soil", f"{local['soil_score']:.1f}")

with t4:
    st.metric("Sound", f"{local['sound_score']:.1f}")

with t5:
    st.metric("Activity", f"{local['pir_score']:.1f}")

with t6:
    st.metric("CO₂", f"{local['co2_ppm']:.0f} ppm")

# ------------------------------------------------------------
# Environmental context
# ------------------------------------------------------------

st.markdown('<div class="section-title">Environmental Context</div>', unsafe_allow_html=True)

e1, e2, e3, e4 = st.columns(4)

with e1:
    st.metric("Rainfall · 1h", f"{environment['rainfall_1h']:.1f} mm")

with e2:
    st.metric("Rainfall · 24h", f"{environment['rainfall_24h']:.1f} mm")

with e3:
    st.metric(
        "Air Quality",
        f"{environment['aqi']:.0f}",
        delta=environment.get("aqi_category", None),
        delta_color="off",
    )

with e4:
    st.metric("Surface Pressure", f"{environment['surface_pressure_hpa']:.1f} hPa")

# ------------------------------------------------------------
# Map
# ------------------------------------------------------------

st.markdown('<div class="section-title">Monitoring Location</div>', unsafe_allow_html=True)

latitude = latest.get("latitude")
longitude = latest.get("longitude")

if latitude is not None and longitude is not None:
    map_data = pd.DataFrame(
        [{"latitude": latitude, "longitude": longitude}]
    )

    layer = pdk.Layer(
        "ScatterplotLayer",
        data=map_data,
        get_position="[longitude, latitude]",
        get_radius=100,
        pickable=True,
    )

    view = pdk.ViewState(
        latitude=float(latitude),
        longitude=float(longitude),
        zoom=13,
        pitch=0,
    )

    st.pydeck_chart(
        pdk.Deck(
            layers=[layer],
            initial_view_state=view,
            tooltip={"text": "GreenPulse monitoring node"},
        ),
        use_container_width=True,
    )

    st.caption(
        f"GPS source: {latest.get('gps_source', 'UNKNOWN')} · "
        f"{float(latitude):.5f}, {float(longitude):.5f}"
    )
else:
    st.warning("Monitoring coordinates are unavailable.")

# ------------------------------------------------------------
# Historical graphs
# ------------------------------------------------------------

st.markdown('<div class="section-title">Recent Telemetry</div>', unsafe_allow_html=True)

if history:
    df = pd.DataFrame(history)
    df["time"] = pd.to_datetime(df["time"])
    df = df.sort_values("time").set_index("time")

    chart_left, chart_right = st.columns(2)

    with chart_left:
        st.caption("Temperature")
        st.line_chart(df[["temperature_c"]])

    with chart_right:
        st.caption("Humidity")
        st.line_chart(df[["humidity_percent"]])

    chart_left, chart_right = st.columns(2)

    with chart_left:
        st.caption("Soil / Sound / Activity scores")
        st.line_chart(
            df[["soil_score", "sound_score", "pir_score"]]
        )

    with chart_right:
        st.caption("CO₂")
        st.line_chart(df[["co2_ppm"]])

else:
    st.info("No historical telemetry available.")

# ------------------------------------------------------------
# Footer / refresh
# ------------------------------------------------------------

st.caption(
    f"Latest telemetry: {latest.get('time', 'unknown')} · "
    f"Auto-refresh: {REFRESH_SECONDS}s"
)

time.sleep(REFRESH_SECONDS)
st.rerun()
