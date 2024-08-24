#pragma once
#include <string_view>
#include <array>

static constexpr std::array<std::string_view, 7U> hitboxes =
{
	"Head",
	"Neck",
	"Chest",
	"Stomach",
	"Arms",
	"Legs",
	"Feet"
};

static constexpr std::array<std::string_view, 7U> multipoint =
{
	"Head",
	"Neck",
	"Chest",
	"Stomach",
	"Arms",
	"Legs",
	"Feet"
};

static constexpr std::array<std::string_view, 2U> AutoStopFlags =
{
	"Between shots", "Force accuracy"
};

static constexpr std::array<std::string_view, 1U> AutoStopConditions =
{
	"Stop if lethal"
};

static constexpr std::array<std::string_view, 3U> PreferBodyAim =
{
	"Lethal within one bullet", "Lethal within two bullet", "While in air" //, "After resolver X misses", "Lower than X Health"
};

static constexpr std::array<std::string_view, 5U> Notifications =
{
	"Damage", "Harmed", "Purchases", "Bomb", "Defuse"
};

static constexpr std::array<std::string_view, 2U> TickbaseManipulation =
{
	"Rapid fire", "Break lag compensation"
};

static constexpr std::array<std::string_view, 22U> AutoBuyWeapons =
{
	"Galil AR",
	"FAMAS",
	"AK-47",
	"M4A4",
	"M4A1-S",
	"SSG08",
	"AUG",
	"SG 553",
	"AWP",
	"SCAR-20",
	"G3SG1",
	"Nova",
	"XM1014",
	"MAG-7",
	"M249",
	"Negev",
	"MAC-10",
	"MP9",
	"MP7",
	"UMP-45",
	"P90",
	"PP-Bizon"
};

static constexpr std::array<std::string_view, 8U> AutoBuySecondary =
{
	"Glock",
	"P2000",
	"USP-S",
	"Dual Berettas",
	"P250",
	"Tec-9",
	"Five-SeveN",
	"Desert Eagle"
};

static constexpr std::array<std::string_view, 11U> AutoBuyMisc =
{
	"Vest",
	"Vest and Helmet",
	"Taser",
	"Defuser",
	"Heavy-Armor",
	"Molotov",
	"Incendiary Grenade",
	"Decoy",
	"Flashbang Grenade",
	"HE Grenade",
	"Smoke Grenade"
};

static constexpr std::array<std::string_view, 7U> PlayerFlags =
{
	"Money",
	"Armor",
	"Scoped",
	"Flashed",
	"Reloading",
	"Bomb carrier",
	"Ping"
};

static constexpr std::array<std::string_view, 5U> OutOfFovlags =
{
	"Name",
	"Box",
	"Health bar",
	"Ammo bar",
	"Danger sign"
};

static constexpr std::array<std::string_view, 4U> ItemFlags =
{
	"Weapon icon",
	"Weapon text",
	"Weapon ammo",
	"Weapon distance"
};

static constexpr std::array<std::string_view, 2U> ProjectileFlags =
{
	"Projectile icon",
	"Projectile text"
};

const char* BoxType[] =
{
	"Default",
	"Corner",
	"3D"
};

static constexpr std::array<std::string_view, 2U> BrightnessAdjustment =
{
	"World modulation",
	"Fullbright"
};

static constexpr std::array<std::string_view, 7U> Indicators =
{
	"Spectator list",
	"Slowed down",
	"Watermark",
	"Rapid fire",
	"Fake lag",
	"Speed",
	"Key bind list"
};

static constexpr std::array<std::string_view, 2U> ActivationType =
{
	"Visible", "Behind wall"
};

static constexpr std::array<std::string_view, 12U> Removals =
{
	"Visual punch", "Visual kick [SOON]", "Scope overlay", "Smoke", "Flashbang", "Teammates" , "Fog", "Sleeves", "Hand shaking [SOON]", "Landing bob [SOON]", "Shadows [SOON]", "Post processing"
};

const char* PenCrosshairType[] = {
	"Simple",
	"Advanced"
};

const char* anti_aim_fake_yaw[] = {
	"Off",
	"Default"
};

const char* anti_aim_pitch[] = {
	"Off",
	"Default"
};

const char* aimbot_priority[] = {
	"Kill",
	"Assist",
	"Accuracy"
};

const char* optimization_level[] = {
	"Off",
	"Low",
	"Medium",
	"High",
	"Maximum"
};

const char* anti_aim_yaw[] = {
	"Off",
	"Default"
};

const char* clantag[] = {
	"Off",
	"Static",
	"Dynamic"
};

const char* auto_stop[] = {
	"Off",
	"Quick"
};

const char* prefer_head_aim[] = {
	"Always",
	"Resolved",
	"Moving",
	"In air"
};

const char* stop_mode[] = {
	"Quick stop",
	"Slide stop"
};

const char* visuals_smoke[] = {
	"Default",
	"Wireframe"
};

const char* anti_onshot[] = {
	"Default",
	"Enable lag on shots",
	"Disable lag on shots"
};

const char* visuals_skybox[] = {
	"Tibet",
	"Embassy",
	"Italy",
	"Daylight",
	"Cloudy",
	"Night 1",
	"Night 2",
	"Night Flat",
	"Day HD",
	"Day",
	"Rural",
	"Vertigo HD",
	"Vertigo Blue HD",
	"Vertigo",
	"Vietnam",
	"Dusty Sky",
	"Jungle",
	"Nuke",
	"Office"
};

const char* visuals_chams_mode[] = {
	"Default",
	"Flat",
	"Ghost",
	"Glass",
	"Pulse"
};

const char* anti_aim_base_angle[] = {
	"Local view",
	"At targets",
	"Static"
};

const char* anti_aim_fakelag_mode[] = {
	"Default",
	"Adaptive",
	"Random",
	"Step"
};

const char* knife_model[] = {
	"Bayonet",
	"Bowie",
	"Butterfly",
	"Falchion",
	"Flip",
	"Gut",
	"Huntsman",
	"Karambit",
	"M9 Bayonet",
	"Shadow daggers"
};

const char* glove_model[] = {
	"Bloodhound",
	"Sport",
	"Driver",
	"Handwraps",
	"Moto",
	"Specialist"
};