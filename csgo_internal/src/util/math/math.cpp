#include "../../precompiled.h"
#include <random>

matrix3x4_t Math::AngleMatrix(const QAngle angles)
{
    matrix3x4_t result{};

    m128 angle, sin, cos;
    angle.f[0] = DEG2RAD(angles.x);
    angle.f[1] = DEG2RAD(angles.y);
    angle.f[2] = DEG2RAD(angles.z);
    sincos_ps(angle.v, &sin.v, &cos.v);

    result[0][0] = cos.f[0] * cos.f[1];
    result[1][0] = cos.f[0] * sin.f[1];
    result[2][0] = -sin.f[0];

    const auto crcy = cos.f[2] * cos.f[1];
    const auto crsy = cos.f[2] * sin.f[1];
    const auto srcy = sin.f[2] * cos.f[1];
    const auto srsy = sin.f[2] * sin.f[1];

    result[0][1] = sin.f[0] * srcy - crsy;
    result[1][1] = sin.f[0] * srsy + crcy;
    result[2][1] = sin.f[2] * cos.f[0];

    result[0][2] = sin.f[0] * crcy + srsy;
    result[1][2] = sin.f[0] * crsy - srcy;
    result[2][2] = cos.f[2] * cos.f[0];

    return result;
}

void Math::AngleMatrix(const QAngle& ang, const Vector& pos, matrix3x4_t& out) {
    g_Csgo.AngleMatrix(ang, out);
    out.SetOrigin(pos);
}

float Math::ClampCycle(float flCycleIn)
{
    flCycleIn -= int(flCycleIn);

    if (flCycleIn < 0)
    {
        flCycleIn += 1;
    }
    else if (flCycleIn > 1)
    {
        flCycleIn -= 1;
    }

    return flCycleIn;
}

float Math::DotProduct(const Vector& a, const Vector& b) {
    return(a.x * b.x + a.y * b.y + a.z * b.z);
}

float Math::DotProduct(const Vector* a, const Vector* b) {
    return(a->x * b->x + a->y * b->y + a->z * b->z);
}

float Math::DistanceToRay(const Vector& pos, const Vector& rayStart, const Vector& rayEnd, float* along, Vector* pointOnRay)
{
    Vector to = pos - rayStart;
    Vector dir = rayEnd - rayStart;
    float Length = dir.Normalize();

    float rangeAlong = dir.Dot(to);
    if (along)
        *along = rangeAlong;

    float range;

    if (rangeAlong < 0.0f)
    {
        range = -(pos - rayStart).Length();

        if (pointOnRay)
            *pointOnRay = rayStart;
    }
    else if (rangeAlong > Length)
    {
        range = -(pos - rayEnd).Length();

        if (pointOnRay)
            *pointOnRay = rayEnd;
    }
    else
    {
        Vector onRay = rayStart + dir * rangeAlong;

        range = (pos - onRay).Length();

        if (pointOnRay)
            *pointOnRay = onRay;
    }

    return range;
}

float Math::AngleNormalize(float angle)
{
    angle = fmodf(angle, 360.0f);
    if (angle > 180.f)
    {
        angle -= 360.f;
    }
    if (angle < -180.f)
    {
        angle += 360.f;
    }
    return angle;
}

unsigned __fastcall Math::HashInt(const int n)
{
    unsigned		even, odd;
    even = g_nRandomValues[n & 0xff];
    odd = g_nRandomValues[((n >> 8) & 0xff)];

    even = g_nRandomValues[odd ^ (n >> 24)];
    odd = g_nRandomValues[even ^ (n >> 16) & 0xff];
    even = g_nRandomValues[odd ^ ((n >> 8) & 0xff)];
    odd = g_nRandomValues[even ^ (n & 0xff)];

    return (even << 8) | odd;
}

unsigned __fastcall Math::HashString(const char* pszKey)
{
    const uint8* k = (const uint8*)pszKey;
    unsigned 	even = 0,
        odd = 0,
        n;

    while ((n = *k++) != 0)
    {
        even = g_nRandomValues[odd ^ n];
        if ((n = *k++) != 0)
            odd = g_nRandomValues[even ^ n];
        else
            break;
    }

    return (even << 8) | odd;
}

unsigned __fastcall Math::HashStringCaseless(const char* pszKey)
{
    const uint8* k = (const uint8*)pszKey;
    unsigned	even = 0,
        odd = 0,
        n;

    while ((n = toupper(*k++)) != 0)
    {
        even = g_nRandomValues[odd ^ n];
        if ((n = toupper(*k++)) != 0)
            odd = g_nRandomValues[even ^ n];
        else
            break;
    }

    return (even << 8) | odd;
}

float Math::NormalizeYaw(float flAngle)
{
    flAngle = fmod(flAngle, 360.0f);
    if (flAngle > 180.0f)
        flAngle -= 360.0f;
    if (flAngle < -180.0f)
        flAngle += 360.0f;

    return flAngle;
}

void Math::NormalizeAngle(float& angle) {
    float rot;

    // bad number.
    if (!std::isfinite(angle)) {
        angle = 0.f;
        return;
    }

    // no need to normalize this angle.
    if (angle >= -180.f && angle <= 180.f)
        return;

    // get amount of rotations needed.
    rot = std::round(std::abs(angle / 360.f));

    // normalize.
    angle = (angle < 0.f) ? angle + (360.f * rot) : angle - (360.f * rot);
}

QAngle Math::CalcAngle(const Vector& vecSource, const Vector& vecDestination) {
    QAngle vAngle;
    Vector Delta((vecSource.x - vecDestination.x), (vecSource.y - vecDestination.y), (vecSource.z - vecDestination.z));
    double hyp = sqrt(Delta.x * Delta.x + Delta.y * Delta.y);

    vAngle.x = float(atanf(float(Delta.z / hyp)) * 57.295779513082f);
    vAngle.y = float(atanf(float(Delta.y / Delta.x)) * 57.295779513082f);
    vAngle.z = 0.0f;

    if (Delta.x >= 0.0)
        vAngle.y += 180.0f;

    return vAngle;
}

float Math::AngleDiff(float destAngle, float srcAngle)
{
    float Delta = fmodf(destAngle - srcAngle, 360.0f);

    if (destAngle > srcAngle)
    {
        if (Delta >= 180)
            Delta -= 360;
    }
    else
    {
        if (Delta <= -180)
            Delta += 360;
    }

    return Delta;
}

float Math::Approach(float target, float value, float speed)
{
    float Delta = target - value;

    if (Delta > speed)
        value += speed;
    else if (Delta < -speed)
        value -= speed;
    else
        value = target;

    return value;
}

Vector Math::Approach(Vector target, Vector value, float speed)
{
    Vector diff = (target - value);
    float Delta = diff.Length();

    if (Delta > speed)
        value += diff.Normalized() * speed;
    else if (Delta < -speed)
        value -= diff.Normalized() * speed;
    else
        value = target;

    return value;
}

float Math::ApproachAngle(float target, float value, float speed) {
    float Delta;

    target = AngleMod(target);
    value = AngleMod(value);
    Delta = target - value;

    // speed is assumed to be positive.
    speed = std::abs(speed);

    Math::NormalizeAngle(Delta);

    if (Delta > speed)
        value += speed;

    else if (Delta < -speed)
        value -= speed;

    else
        value = target;

    return value;
}

void Math::RotatePoint(Vector2D& point, Vector2D origin, bool clockwise, float angle) {
    Vector2D Delta = point - origin;
    Vector2D rotated;

    if (clockwise)
        rotated = Vector2D(Delta.x * cosf(angle) - Delta.y * sinf(angle), Delta.x * sinf(angle) + Delta.y * cosf(angle));
    else
        rotated = Vector2D(Delta.x * sinf(angle) - Delta.y * cosf(angle), Delta.x * cosf(angle) + Delta.y * sinf(angle));

    point = rotated + origin;
}

inline void CrossProduct(const Vector& a, const Vector& b, Vector& out)
{
    out = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

void __fastcall _VectorNormalizeFast(Vector& vec)
{
    // FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
    float iradius = 1.f / (sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z) + FLT_EPSILON);

    vec.x *= iradius;
    vec.y *= iradius;
    vec.z *= iradius;
}

__forceinline float __vectorcall FastRSqrtFast(float x)
{
    // use intrinsics
    __m128 rroot = _mm_rsqrt_ss(_mm_load_ss(&x));
    return *(reinterpret_cast<float*>(&rroot));
}

__forceinline float __vectorcall FastRSqrt(float x)
{
    float rroot = FastRSqrtFast(x);
    return (0.5f * rroot) * (3.f - (x * rroot) * rroot);
}

inline void _SSE_RSqrtInline(float a, float* out)
{
    __m128  xx = _mm_load_ss(&a);
    __m128  xr = _mm_rsqrt_ss(xx);
    __m128  xt;
    xt = _mm_mul_ss(xr, xr);
    xt = _mm_mul_ss(xt, xx);
    xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
    xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
    xr = _mm_mul_ss(xr, xt);
    _mm_store_ss(out, xr);
}

// FIXME: Change this back to a #define once we get rid of the vec_t version
__forceinline void VectorNormalizeFast(Vector& vec)
{
#if 0
#ifndef DEBUG // stop crashing my edit-and-continue!
#if defined(__i386__) || defined(_M_IX86)
#define DO_SSE_OPTIMIZATION
#endif
#endif
#endif
#define DO_SSE_OPTIMIZATION

#if defined( DO_SSE_OPTIMIZATION )
    float sqrlen = vec.LengthSqr() + 1.0e-10f, invlen;
    _SSE_RSqrtInline(sqrlen, &invlen);
    vec.x *= invlen;
    vec.y *= invlen;
    vec.z *= invlen;
#else
    extern float (FASTCALL * pfVectorNormalize)(Vector & v);
    return (*pfVectorNormalize)(vec);
#endif
}

void Math::NormalizeVector(Vector& vec)
{
    for (int i = 0; i < 2; ++i)
    {
        while (vec[i] > 180.f)
            vec[i] -= 360.f;

        while (vec[i] < -180.f)
            vec[i] += 360.f;
    }

    vec[2] = 0.f;
}

void Math::VectorAngles(const Vector& forward, const Vector& pseudoup, QAngle& angles)
{
    Vector left;

    CrossProduct(pseudoup, forward, left);
    VectorNormalizeFast(left);

    float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

    // enough here to get angles?
    if (xyDist > 0.001f)
    {
        // (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
        angles[1] = RAD2DEG(atan2f(forward[1], forward[0]));

        // The engine does pitch inverted from this, but we always end up negating it in the DLL
        // UNDONE: Fix the engine to make it consistent
        // (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
        angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

        float up_z = (left[1] * forward[0]) - (left[0] * forward[1]);

        // (roll)	z = ATAN( left.z, up.z );
        angles[2] = RAD2DEG(atan2f(left[2], up_z));
    }
    else	// forward is mostly Z, gimbal lock-
    {
        // (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
        angles[1] = RAD2DEG(atan2f(-left[0], left[1])); //This was originally copied from the "void MatrixAngles( const matrix3x4_t& matrix, float *angles )" code, and it's 180 degrees off, negated the values and it all works now (Dave Kircher)

        // The engine does pitch inverted from this, but we always end up negating it in the DLL
        // UNDONE: Fix the engine to make it consistent
        // (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
        angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

        // Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
        angles[2] = 0;
    }
}

void Math::VectorAngles(const Vector& forward, QAngle& angles, Vector* up) {
    Vector  left;
    float   len, up_z, pitch, yaw, roll;

    // get 2d length.
    len = forward.Length2D();

    if (up && len > 0.001f) {
        pitch = RAD2DEG(std::atan2(-forward.z, len));
        yaw = RAD2DEG(std::atan2(forward.y, forward.x));

        // get left direction vector using Cross product.
        left = (*up).Cross(forward).Normalized();

        // calculate up_z.
        up_z = (left.y * forward.x) - (left.x * forward.y);

        // calculate roll.
        roll = RAD2DEG(std::atan2(left.z, up_z));
    }

    else {
        if (len > 0.f) {
            // calculate pitch and yaw.
            pitch = RAD2DEG(std::atan2(-forward.z, len));
            yaw = RAD2DEG(std::atan2(forward.y, forward.x));
            roll = 0.f;
        }

        else {
            pitch = (forward.z > 0.f) ? -90.f : 90.f;
            yaw = 0.f;
            roll = 0.f;
        }
    }

    // set out angles.
    angles = { pitch, yaw, roll };
}

void Math::AngleVectors(const QAngle& angles, Vector* forward, Vector* right, Vector* up) {
    float sr, sp, sy, cr, cp, cy;

    SinCos(DEG2RAD(angles.x), sp, cp);
    SinCos(DEG2RAD(angles.y), sy, cy);
    SinCos(DEG2RAD(angles.z), sr, cr);

    if (forward) {
        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    }

    if (right) {
        right->x = -1.f * sr * sp * cy + -1.f * cr * -sy;
        right->y = -1.f * sr * sp * sy + -1.f * cr * cy;
        right->z = -1.f * sr * cp;
    }

    if (up) {
        up->x = cr * sp * cy + -sr * -sy;
        up->y = cr * sp * sy + -sr * cy;
        up->z = cr * cp;
    }
}

void Math::SinCos(const float x, float& sin, float& cos) noexcept
{
    sin = std::sin(x);
    cos = std::cos(x);
}

float Math::GetFOV(const QAngle& aimAngle, const QAngle& viewAngle)
{
    // get direction and normalize.
    QAngle Delta = (aimAngle - viewAngle).Normalized();

    // get the angle between the view angles forward directional vector and the target location.
    return sqrtf(powf(Delta.x, 2.0f) + powf(Delta.y, 2.0f));
}

void Math::VectorTransform(const Vector& in, const matrix3x4_t& matrix, Vector& out) {
    out = {
        in.Dot(Vector(matrix[0][0], matrix[0][1], matrix[0][2])) + matrix[0][3],
        in.Dot(Vector(matrix[1][0], matrix[1][1], matrix[1][2])) + matrix[1][3],
        in.Dot(Vector(matrix[2][0], matrix[2][1], matrix[2][2])) + matrix[2][3]
    };
}

float Math::SegmentToSegment(const Vector s1, const Vector s2, const Vector k1, const Vector k2)
{
    static auto constexpr epsilon = 0.00000001;

    auto u = s2 - s1;
    auto v = k2 - k1;
    const auto w = s1 - k1;

    const auto a = u.Dot(u);
    const auto b = u.Dot(v);
    const auto c = v.Dot(v);
    const auto d = u.Dot(w);
    const auto e = v.Dot(w);
    const auto D = a * c - b * b;
    float sn, sd = D;
    float tn, td = D;

    if (D < epsilon) {
        sn = 0.0;
        sd = 1.0;
        tn = e;
        td = c;
    }
    else {
        sn = b * e - c * d;
        tn = a * e - b * d;

        if (sn < 0.0) {
            sn = 0.0;
            tn = e;
            td = c;
        }
        else if (sn > sd) {
            sn = sd;
            tn = e + b;
            td = c;
        }
    }

    if (tn < 0.0) {
        tn = 0.0;

        if (-d < 0.0)
            sn = 0.0;
        else if (-d > a)
            sn = sd;
        else {
            sn = -d;
            sd = a;
        }
    }
    else if (tn > td) {
        tn = td;

        if (-d + b < 0.0)
            sn = 0;
        else if (-d + b > a)
            sn = sd;
        else {
            sn = -d + b;
            sd = a;
        }
    }

    const float sc = abs(sn) < epsilon ? 0.0 : sn / sd;
    const float tc = abs(tn) < epsilon ? 0.0 : tn / td;

    m128 n;
    auto dp = w + u * sc - v * tc;
    n.f[0] = dp.Dot(dp);
    const auto calc = sqrt_ps(n.v);
    return reinterpret_cast<const m128*>(&calc)->f[0];

}

void Math::VectorITransform(const Vector& in, const matrix3x4_t& matrix, Vector& out) {
    Vector diff;

    diff = {
        in.x - matrix[0][3],
        in.y - matrix[1][3],
        in.z - matrix[2][3]
    };

    out = {
        diff.x * matrix[0][0] + diff.y * matrix[1][0] + diff.z * matrix[2][0],
        diff.x * matrix[0][1] + diff.y * matrix[1][1] + diff.z * matrix[2][1],
        diff.x * matrix[0][2] + diff.y * matrix[1][2] + diff.z * matrix[2][2]
    };
}

void Math::MatrixAngles(const matrix3x4_t& matrix, QAngle& angles) {
    Vector forward, left, up;

    // extract the basis vectors from the matrix. since we only need the z
    // component of the up vector, we don't get x and y.
    forward = { matrix[0][0], matrix[1][0], matrix[2][0] };
    left = { matrix[0][1], matrix[1][1], matrix[2][1] };
    up = { 0.f, 0.f, matrix[2][2] };

    float len = forward.Length2D();

    // enough here to get angles?
    if (len > 0.001f) {
        angles.x = RAD2DEG(std::atan2(-forward.z, len));
        angles.y = RAD2DEG(std::atan2(forward.y, forward.x));
        angles.z = RAD2DEG(std::atan2(left.z, up.z));
    }

    else {
        angles.x = RAD2DEG(std::atan2(-forward.z, len));
        angles.y = RAD2DEG(std::atan2(-left.x, left.y));
        angles.z = 0.f;
    }
}

void Math::MatrixPosition(const matrix3x4_t& matrix, Vector& position)
{
    position[0] = matrix[0][3];
    position[1] = matrix[1][3];
    position[2] = matrix[2][3];
}

void Math::MatrixAngles(const matrix3x4_t& matrix, QAngle& angles, Vector& position)
{
    MatrixAngles(matrix, angles);
    MatrixPosition(matrix, position);
}

void Math::MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out) {
    std::memcpy(out.Base(), in.Base(), sizeof(matrix3x4_t));
}

void Math::ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out) {
    if (&in1 == &out) {
        matrix3x4_t in1b;
        MatrixCopy(in1, in1b);
        ConcatTransforms(in1b, in2, out);
        return;
    }

    if (&in2 == &out) {
        matrix3x4_t in2b;
        MatrixCopy(in2, in2b);
        ConcatTransforms(in1, in2b, out);
        return;
    }

    out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
    out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
    out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
    out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];

    out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
    out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
    out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
    out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];

    out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
    out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
    out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
    out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}

bool Math::IntersectRayWithBox(const Vector& start, const Vector& Delta, const Vector& mins, const Vector& maxs, float tolerance, BoxTraceInfo_t* out_info) {
    int   i;
    float d1, d2, f;

    for (i = 0; i < 6; ++i) {
        if (i >= 3) {
            d1 = start[i - 3] - maxs[i - 3];
            d2 = d1 + Delta[i - 3];
        }

        else {
            d1 = -start[i] + mins[i];
            d2 = d1 - Delta[i];
        }

        // if completely in front of face, no intersection.
        if (d1 > 0.f && d2 > 0.f) {
            out_info->m_StartSolid = false;

            return false;
        }

        // completely inside, check next face.
        if (d1 <= 0.f && d2 <= 0.f)
            continue;

        if (d1 > 0.f)
            out_info->m_StartSolid = false;

        // crosses face.
        if (d1 > d2) {
            f = std::max(0.f, d1 - tolerance);

            f = f / (d1 - d2);
            if (f > out_info->m_T1) {
                out_info->m_T1 = f;
                out_info->m_HitSide = i;
            }
        }

        // leave.
        else {
            f = (d1 + tolerance) / (d1 - d2);
            if (f < out_info->m_T2)
                out_info->m_T2 = f;
        }
    }

    return out_info->m_StartSolid || (out_info->m_T1 < out_info->m_T2&& out_info->m_T1 >= 0.f);
}

bool Math::IntersectRayWithBox(const Vector& start, const Vector& Delta, const Vector& mins, const Vector& maxs, float tolerance, CBaseTrace* out_tr, float* fraction_left_solid) {
    BoxTraceInfo_t box_tr;

    // note - dex; this is Collision_ClearTrace.
    out_tr->m_StartPosition = start;
    out_tr->m_EndPosition = start;
    out_tr->m_EndPosition += Delta;
    out_tr->m_StartSolid = false;
    out_tr->m_AllSolid = false;
    out_tr->m_Fraction = 1.f;
    out_tr->m_Contents = 0;

    if (IntersectRayWithBox(start, Delta, mins, maxs, tolerance, &box_tr)) {
        out_tr->m_StartSolid = box_tr.m_StartSolid;

        if (box_tr.m_T1 < box_tr.m_T2 && box_tr.m_T1 >= 0.f) {
            out_tr->m_Fraction = box_tr.m_T1;

            // VectorMA( pTrace->startpos, trace.t1, vecRayDelta, pTrace->endpos );

            out_tr->m_Contents = CONTENTS_SOLID;
            out_tr->m_Plane.m_Normal = Vector{};

            if (box_tr.m_HitSide >= 3) {
                box_tr.m_HitSide -= 3;

                out_tr->m_Plane.m_Distance = maxs[box_tr.m_HitSide];
                out_tr->m_Plane.m_Normal[box_tr.m_HitSide] = 1.f;
                out_tr->m_Plane.m_Type = box_tr.m_HitSide;
            }

            else {
                out_tr->m_Plane.m_Distance = -mins[box_tr.m_HitSide];
                out_tr->m_Plane.m_Normal[box_tr.m_HitSide] = -1.f;
                out_tr->m_Plane.m_Type = box_tr.m_HitSide;
            }

            return true;
        }

        if (out_tr->m_StartSolid) {
            out_tr->m_AllSolid = (box_tr.m_T2 <= 0.f) || (box_tr.m_T2 >= 1.f);
            out_tr->m_Fraction = 0.f;

            if (fraction_left_solid)
                *fraction_left_solid = box_tr.m_T2;

            out_tr->m_EndPosition = out_tr->m_StartPosition;
            out_tr->m_Contents = CONTENTS_SOLID;
            out_tr->m_Plane.m_Distance = out_tr->m_StartPosition.x;
            out_tr->m_Plane.m_Normal = { 1.f, 0.f, 0.f };
            out_tr->m_Plane.m_Type = 0;
            out_tr->m_StartPosition = start + (box_tr.m_T2 * Delta);

            return true;
        }
    }

    return false;
}

bool Math::IntersectRayWithOBB(const Vector& start, const Vector& Delta, const matrix3x4_t& obb_to_world, const Vector& mins, const Vector& maxs, float tolerance, CBaseTrace* out_tr) {
    Vector box_extents, box_center, extent{}, uextent, segment_center, Cross, new_start, tmp_end;
    float  coord, tmp, cextent, sign;

    // note - dex; this is Collision_ClearTrace.
    out_tr->m_StartPosition = start;
    out_tr->m_EndPosition = start;
    out_tr->m_EndPosition += Delta;
    out_tr->m_StartSolid = false;
    out_tr->m_AllSolid = false;
    out_tr->m_Fraction = 1.f;
    out_tr->m_Contents = 0;

    // compute center in local space and transform to world space.
    box_extents = (mins + maxs) / 2.f;
    VectorTransform(box_extents, obb_to_world, box_center);

    // calculate extents from local center.
    box_extents = maxs - box_extents;

    // save the extents of the ray.
    segment_center = start + Delta - box_center;

    // check box axes for separation.
    for (int i = 0; i < 3; ++i) {
        extent[i] = Delta.x * obb_to_world[0][i] + Delta.y * obb_to_world[1][i] + Delta.z * obb_to_world[2][i];
        uextent[i] = std::abs(extent[i]);

        coord = segment_center.x * obb_to_world[0][i] + segment_center.y * obb_to_world[1][i] + segment_center.z * obb_to_world[2][i];
        coord = std::abs(coord);
        if (coord > (box_extents[i] + uextent[i]))
            return false;
    }

    // now check cross axes for separation.
    Cross = Delta.Cross(segment_center);

    cextent = Cross.x * obb_to_world[0][0] + Cross.y * obb_to_world[1][0] + Cross.z * obb_to_world[2][0];
    cextent = std::abs(cextent);
    tmp = box_extents.y * uextent.z + box_extents.z * uextent.y;
    if (cextent > tmp)
        return false;

    cextent = Cross.x * obb_to_world[0][1] + Cross.y * obb_to_world[1][1] + Cross.z * obb_to_world[2][1];
    cextent = std::abs(cextent);
    tmp = box_extents.x * uextent.z + box_extents.z * uextent.x;
    if (cextent > tmp)
        return false;

    cextent = Cross.x * obb_to_world[0][2] + Cross.y * obb_to_world[1][2] + Cross.z * obb_to_world[2][2];
    cextent = std::abs(cextent);
    tmp = box_extents.x * uextent.y + box_extents.y * uextent.x;
    if (cextent > tmp)
        return false;

    // we hit this box, compute intersection point and return.
    // compute ray start in bone space.
    VectorITransform(start, obb_to_world, new_start);

    // extent is ray.m_Delta in bone space, recompute delta in bone space.
    extent *= 2.f;

    // delta was prescaled by the current t, so no need to see if this intersection is closer.
    if (!IntersectRayWithBox(start, extent, mins, maxs, tolerance, out_tr))
        return false;

    // fix up the start/end pos and fraction
    VectorTransform(out_tr->m_EndPosition, obb_to_world, tmp_end);

    out_tr->m_EndPosition = tmp_end;
    out_tr->m_StartPosition = start;
    out_tr->m_Fraction *= 2.f;

    // fix up the plane information
    sign = out_tr->m_Plane.m_Normal[out_tr->m_Plane.m_Type];

    out_tr->m_Plane.m_Normal.x = sign * obb_to_world[0][out_tr->m_Plane.m_Type];
    out_tr->m_Plane.m_Normal.y = sign * obb_to_world[1][out_tr->m_Plane.m_Type];
    out_tr->m_Plane.m_Normal.z = sign * obb_to_world[2][out_tr->m_Plane.m_Type];
    out_tr->m_Plane.m_Distance = out_tr->m_EndPosition.Dot(out_tr->m_Plane.m_Normal);
    out_tr->m_Plane.m_Type = 3;

    return true;
}

bool Math::IntersectRayWithOBB(const Vector& start, const Vector& Delta, const Vector& box_origin, const QAngle& box_rotation, const Vector& mins, const Vector& maxs, float tolerance, CBaseTrace* out_tr) {
    // todo - dex; https://github.com/pmrowla/hl2sdk-csgo/blob/master/public/collisionutils.cpp#L1400
    return false;
}

bool Math::IntersectInfiniteRayWithSphere(const Vector& start, const Vector& Delta, const Vector& sphere_center, float radius, float* out_t1, float* out_t2) {
    Vector sphere_to_ray;
    float  a, b, c, discrim, oo2a;

    sphere_to_ray = start - sphere_center;
    a = Delta.Dot(Delta);

    // this would occur in the case of a zero-Length ray.
    if (!a) {
        *out_t1 = 0.f;
        *out_t2 = 0.f;

        return sphere_to_ray.LengthSqr() <= (radius * radius);
    }

    b = 2.f * sphere_to_ray.Dot(Delta);
    c = sphere_to_ray.Dot(sphere_to_ray) - (radius * radius);

    discrim = b * b - 4.f * a * c;
    if (discrim < 0.f)
        return false;

    discrim = std::sqrt(discrim);
    oo2a = 0.5f / a;

    *out_t1 = (-b - discrim) * oo2a;
    *out_t2 = (-b + discrim) * oo2a;

    return true;
}

// Rotate a vector around the Z axis (YAW)
void Math::VectorYawRotate(const Vector& in, float flYaw, Vector& out)
{
    if (&in == &out)
    {
        Vector tmp;
        tmp = in;
        VectorYawRotate(tmp, flYaw, out);
        return;
    }

    float sy, cy;

    SinCos(DEG2RAD(flYaw), sy, cy);

    out.x = in.x * cy - in.y * sy;
    out.y = in.x * sy + in.y * cy;
    out.z = in.z;
}

bool Math::IntersectRayWithSphere(const Vector& start, const Vector& Delta, const Vector& sphere_center, float radius, float* out_t1, float* out_t2) {
    if (!IntersectInfiniteRayWithSphere(start, Delta, sphere_center, radius, out_t1, out_t2))
        return false;

    if (*out_t1 > 1.0f || *out_t2 < 0.0f)
        return false;

    // clamp intersection points.
    *out_t1 = std::max(0.f, *out_t1);
    *out_t2 = std::min(1.f, *out_t2);

    return true;
}

bool Math::IntersectLineWithBB(Vector& start, Vector& end, Vector& min, Vector& max) {
    float d1, d2, f;
    auto start_solid = true;
    auto t1 = -1.0f, t2 = 1.0f;

    const float s[3] = { start.x, start.y, start.z };
    const float e[3] = { end.x, end.y, end.z };
    const float mi[3] = { min.x, min.y, min.z };
    const float ma[3] = { max.x, max.y, max.z };

    for (auto i = 0; i < 6; i++) {
        if (i >= 3) {
            const auto j = i - 3;

            d1 = s[j] - ma[j];
            d2 = d1 + e[j];
        }
        else {
            d1 = -s[i] + mi[i];
            d2 = d1 - e[i];
        }

        if (d1 > 0.0f && d2 > 0.0f)
            return false;

        if (d1 <= 0.0f && d2 <= 0.0f)
            continue;

        if (d1 > 0)
            start_solid = false;

        if (d1 > d2) {
            f = d1;
            if (f < 0.0f)
                f = 0.0f;

            f /= d1 - d2;
            if (f > t1)
                t1 = f;
        }
        else {
            f = d1 / (d1 - d2);
            if (f < t2)
                t2 = f;
        }
    }

    return start_solid || (t1 < t2&& t1 >= 0.0f);
}

Vector Math::Interpolate(const Vector from, const Vector to, const float percent) {
    return to * percent + from * (1.f - percent);
}

Vector Math::VectorRotate(Vector& in1, matrix3x4_t& in2)
{
    return Vector(in1.Dot(in2[0]), in1.Dot(in2[1]), in1.Dot(in2[2]));
}

Vector Math::VectorRotate(Vector& in1, QAngle& in2)
{
    auto matrix = AngleMatrix(in2);
    return VectorRotate(in1, matrix);
}
bool Math::WorldToScreen(const Vector& in, Vector2D& out)
{
    const auto ScreenTransform = [](const Vector& in, Vector2D& out) -> bool
    {
        static auto& W2SMatrix = g_Csgo.m_Engine->WorldToScreenMatrix();

        out.x = W2SMatrix.m[0][0] * in.x + W2SMatrix.m[0][1] * in.y + W2SMatrix.m[0][2] * in.z + W2SMatrix.m[0][3];
        out.y = W2SMatrix.m[1][0] * in.x + W2SMatrix.m[1][1] * in.y + W2SMatrix.m[1][2] * in.z + W2SMatrix.m[1][3];

        const float w = W2SMatrix.m[3][0] * in.x + W2SMatrix.m[3][1] * in.y + W2SMatrix.m[3][2] * in.z + W2SMatrix.m[3][3];

        if (w < 0.001f)
        {
            out.x *= 100000.0f;
            out.y *= 100000.0f;
            return false;
        }

        out.x /= w;
        out.y /= w;

        return true;
    };

    if (ScreenTransform(in, out))
    {
        int w = 0, h = 0;
        g_Csgo.m_Engine->GetScreenSize(w, h);

        out.x = (w / 2.0f) + (out.x * w) / 2.0f;
        out.y = (h / 2.0f) - (out.y * h) / 2.0f;

        return true;
    }

    return false;
}