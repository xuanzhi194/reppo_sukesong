/**
 * screen_keep olive-tree care (lives in main, not ui_custom/custom).
 * Strong symbols override the Guider-safe weak stubs in custom.c.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "custom.h"
#include "keep_machine.h"
#ifdef ESP_PLATFORM
#include "nvs.h"
#include "rx8130.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#define KEEP_LEVEL_COUNT    10
#define KEEP_TALK_PER_LEVEL 50
#define KEEP_WITHER_DAYS    1
#define KEEP_NVS_NS         "Keep"
#define KEEP_NVS_KEY        "last_ymd"

/* Cumulative focus hours to reach each level (same table as the old AI page). */
static const int keep_level_hour[KEEP_LEVEL_COUNT] = {
    3, 9, 15, 30, 60, 100, 150, 230, 350, 500
};

static const char * const keep_state_names[KEEP_LEVEL_COUNT] = {
    "First Sprout",
    "Slender Stem",
    "Unfurling Leaf",
    "Young Branch",
    "Reaching Skyward",
    "Lush Foliage",
    "Gentle Bloom",
    "Tender Fruit",
    "Olive Crown",
    "Timeless Roots",
};

static const char * const keep_talk_ok[KEEP_LEVEL_COUNT][KEEP_TALK_PER_LEVEL] = {
    {
        "I just broke the soil. Hi.",
        "A tiny olive tip. Stay close.",
        "Soft light. I made it out.",
        "One green spark, all new.",
        "The earth let me through.",
        "Hello from under the dust.",
        "I am small. I am here.",
        "A first breath of sun.",
        "Don't blink. I just arrived.",
        "Warm dirt. Then you.",
        "I poked the world today.",
        "Still sandy, still smiling.",
        "Keep me in this light.",
        "A sprout saying thanks.",
        "I found the sky. Barely.",
        "Tiny, but I chose you.",
        "The morning woke me up.",
        "One leaf-dream, starting now.",
        "Hold this quiet with me.",
        "I rose for a little care.",
        "Soil on my tip. Hello.",
        "Your time made a crack.",
        "I am new to all this.",
        "Stay. I just learned air.",
        "A green hello, very small.",
        "I waited under. Now I'm up.",
        "First light feels kind.",
        "Don't step back too far.",
        "I can see you now.",
        "A sprout needs a friend.",
        "The ground was heavy. I won.",
        "Today I exist. That's enough.",
        "Whisper. I'm still tender.",
        "You came. I uncurled.",
        "A pin of green for you.",
        "I taste rain already.",
        "New roots, new hope.",
        "This is my first morning.",
        "I broke through for us.",
        "Soft sprout, loud joy.",
        "I fit in your shadow.",
        "The day is huge. I'm not.",
        "Thank you for the hour.",
        "I learned the word sun.",
        "A shy olive, just born.",
        "Come closer. I'm peeking.",
        "I made it. Really.",
        "Green, and a little brave.",
        "Sit with my first day.",
        "I grow if you stay.",
    },
    {
        "My stem can stand today.",
        "Still thin, but I stood up.",
        "A slender stem, thanks to you.",
        "Look, I'm a little taller.",
        "I found my spine.",
        "Wind tests me. I stay.",
        "Thin green, real courage.",
        "I lean toward you.",
        "A reed of olive hope.",
        "I learned not to flop.",
        "Your focus is my stake.",
        "I stand, then I smile.",
        "Height is new to me.",
        "A stem, not a thread.",
        "I hold my own head now.",
        "Don't laugh. This is tall.",
        "I sway, I don't fall.",
        "Slender, and proud of it.",
        "The soil feels farther.",
        "I grew a little pole.",
        "Stand with me a minute.",
        "I practice being upright.",
        "Your hour made me firm.",
        "I can face the breeze.",
        "A green line in the light.",
        "I stopped crawling up.",
        "Look down. I'm rising.",
        "This stem remembers you.",
        "I balance on kindness.",
        "Thin, but not fragile now.",
        "I keep my chin up.",
        "A quiet lift in me.",
        "I stand because you did.",
        "Morning stretch, olive style.",
        "I found a posture.",
        "Still skinny. Still growing.",
        "Hold this height with me.",
        "I won't fold today.",
        "A stem saying I can.",
        "Taller than yesterday. Barely.",
        "I like this new air.",
        "Your care is my rod.",
        "I stand in your time.",
        "A slim yes to the sun.",
        "I grew a little backbone.",
        "Stay. I'll stand longer.",
        "The stem learned your name.",
        "Upright, and a bit shy.",
        "I reach, then I rest.",
        "This is me, standing.",
    },
    {
        "A new leaf just opened.",
        "I unfurled a little green.",
        "Feel that? Fresh leaf air.",
        "My first leaf likes your time.",
        "I learned how to open.",
        "A leaf for the light.",
        "I flattened a tiny sail.",
        "Green cloth, still creased.",
        "I drink sun in sips.",
        "Look, a real leaf face.",
        "I unrolled for you.",
        "Soft veins, new work.",
        "This leaf is my hand.",
        "I can catch dew now.",
        "An olive page, blank.",
        "I opened. Don't rush me.",
        "Shade starts with one leaf.",
        "I feel the breeze better.",
        "A leaf saying good morning.",
        "I practiced unfolding.",
        "Your hour woke this leaf.",
        "Green, wider than before.",
        "I hold a drop of sky.",
        "The leaf likes your voice.",
        "I stopped being only stem.",
        "Unfurling feels like a hug.",
        "A shy leaf, fully out.",
        "I can photosynthesize joy.",
        "This green is brand new.",
        "Stay. I'll open more.",
        "I found my first shade.",
        "A leaf that remembers you.",
        "I stretch without tearing.",
        "Morning leaf, still damp.",
        "I turned toward your time.",
        "One leaf, many thanks.",
        "I rustle, very quietly.",
        "The sun fits on me.",
        "I grew a little flag.",
        "Open, and a bit proud.",
        "This leaf waited for you.",
        "I collect warmth now.",
        "A green ear for the day.",
        "I uncurled on purpose.",
        "Feel my first real leaf.",
        "I made a tiny roof.",
        "Your focus opened me.",
        "I breathe wider today.",
        "A leaf, not a hint.",
        "Come see. I unfolded.",
    },
    {
        "A young branch, just for you.",
        "I grew a real twig today.",
        "Green wood, still soft.",
        "I'm branching out slowly.",
        "A fork in my story.",
        "I have a side now.",
        "This twig chose a path.",
        "Young wood, old patience.",
        "I reach left and right.",
        "A branch to hang hope.",
        "I split, then I smiled.",
        "Soft bark, new plans.",
        "Your time grew a limb.",
        "I can hold a bird soon.",
        "A twig saying try me.",
        "I lean into more sky.",
        "Green elbows, still tender.",
        "I learned a new direction.",
        "This branch is my yes.",
        "I grew a little road.",
        "Stay. I'll fork again.",
        "Young wood likes your hour.",
        "I am more than upright.",
        "A branch practicing reach.",
        "I hold two futures now.",
        "Soft snap? No. I'm fine.",
        "I built a side room.",
        "This twig remembers you.",
        "I stretch across the light.",
        "A young limb, quite brave.",
        "I can shade a finger.",
        "Branching feels like sharing.",
        "I grew an extra hello.",
        "The wind has more of me.",
        "I chose a second way.",
        "Green wood, real work.",
        "Your care is in this twig.",
        "I am becoming a tree.",
        "A branch for later fruit.",
        "I reach without rushing.",
        "Look, I have architecture.",
        "I made a little arm.",
        "This is my first fork.",
        "Young, and already wider.",
        "I hold space for leaves.",
        "A twig full of plans.",
        "I grew sideways for you.",
        "Come see my new limb.",
        "I practice being a grove.",
        "This branch is ours.",
    },
    {
        "I'm reaching for the light.",
        "Skyward, a little more.",
        "Your focus pulls me up.",
        "I want the sun with you.",
        "The sky feels closer.",
        "I climb without hurry.",
        "Up is my favorite word.",
        "I stretch into blue.",
        "A taller yes today.",
        "I chase a warmer ray.",
        "Your hour is my lift.",
        "I grow against the gravity.",
        "Look up. I'm trying.",
        "I aim at noon.",
        "Sky in my veins.",
        "I reach, then I rest.",
        "Higher than last week.",
        "I learned the word above.",
        "A lift in every cell.",
        "I follow the bright.",
        "Don't pull me down yet.",
        "I want that high air.",
        "Your time is scaffolding.",
        "I grow like a wish.",
        "The light called. I came.",
        "I stand taller for you.",
        "Skyward, still gentle.",
        "I keep the sun in sight.",
        "A long stretch, olive style.",
        "I rise on quiet days.",
        "This height is shared.",
        "I climb your focus.",
        "More sky, more thanks.",
        "I point at tomorrow.",
        "Up, and a little proud.",
        "I leave the shade behind.",
        "A green arrow, slow.",
        "I reach because you stay.",
        "The noon likes me now.",
        "I grow toward your hour.",
        "Higher, not hurried.",
        "I keep lifting my crown.",
        "A skyward habit, new.",
        "I want the top light.",
        "Stay. I'll climb more.",
        "I stretch for both of us.",
        "The air is thinner. Good.",
        "I rise on small tasks.",
        "Look, less ground, more sky.",
        "I am a reaching thing.",
    },
    {
        "Lush leaves, still young.",
        "So much green around me.",
        "The foliage feels full.",
        "Shade and silver, both mine.",
        "I wear a green coat.",
        "Leaves talking in wind.",
        "I made a little room.",
        "Full, and still growing.",
        "A hush under my leaves.",
        "I can hide a secret.",
        "Silver backs, green faces.",
        "Your hour thickened me.",
        "I rustle on purpose.",
        "A canopy, almost.",
        "I hold cool air now.",
        "So many leaves, one thanks.",
        "I shade my own roots.",
        "Green crowd, happy noise.",
        "I dress in olive light.",
        "The wind plays me now.",
        "I grew a soft wall.",
        "Lush, not lazy.",
        "I keep a pocket of dusk.",
        "Leaves like small hands.",
        "I am fuller than fear.",
        "Your time is in this green.",
        "I whisper with my foliage.",
        "A thick hello today.",
        "I catch more morning.",
        "Shade for you, if you sit.",
        "I layered myself in leaves.",
        "Green, then more green.",
        "I feel wealthy in chlorophyll.",
        "A hush of many leaves.",
        "I made my own weather.",
        "Stay under me a bit.",
        "I rustle your name.",
        "Full leaves, calm heart.",
        "I keep the heat off you.",
        "This foliage remembers you.",
        "I grew a quiet tent.",
        "So green I surprise me.",
        "I collect little shadows.",
        "A lush pause in the day.",
        "I wear yesterday's hours.",
        "Leaves enough to share.",
        "I am a small woodland.",
        "Come into this green.",
        "I thicken with your care.",
        "This is my leafy yes.",
    },
    {
        "A quiet bloom, just now.",
        "White petals if you stay.",
        "I flowered in silence.",
        "Soft bloom. Don't rush me.",
        "A pale yes on the twig.",
        "I opened without noise.",
        "Petals like small notes.",
        "I bloom for the hour.",
        "A gentle white hello.",
        "I held a flower back.",
        "Then I let it show.",
        "Your focus is the bee.",
        "I bloom in low voice.",
        "Soft, not showy.",
        "A bloom that waits.",
        "I learned a quiet color.",
        "White against my green.",
        "I flower when you come.",
        "A hush, then a petal.",
        "I don't shout this bloom.",
        "Stay. I'll open slowly.",
        "A bloom for later fruit.",
        "I keep this tender.",
        "The flower knows your time.",
        "I bloomed between tasks.",
        "Soft light on new petals.",
        "I made a tiny festival.",
        "No parade. Just bloom.",
        "I flower in your shade.",
        "A pale crown, starting.",
        "I saved this for you.",
        "Quiet bloom, loud thanks.",
        "I open, I don't perform.",
        "The wind is careful now.",
        "I wear one white thought.",
        "A bloom that remembers you.",
        "I flowered after the wait.",
        "Gentle, like your hour.",
        "I keep the petals cool.",
        "This bloom is not loud.",
        "I learned to be soft.",
        "A white pause in green.",
        "I bloom because you stayed.",
        "Come see, but whisper.",
        "I hold a quiet spring.",
        "Petals, then patience.",
        "I flower without hurry.",
        "This is my soft season.",
        "A bloom, then I rest.",
        "I opened. That's the news.",
    },
    {
        "A tender fruit is here.",
        "Green olives take time.",
        "Warm fruit, still young.",
        "I held a little olive.",
        "Not ripe. Already loved.",
        "A green bead on the twig.",
        "I grew a small promise.",
        "Fruit, still learning sun.",
        "I keep it warm for you.",
        "A tender yes, edible soon.",
        "Don't pick yet. Stay.",
        "I made a tiny globe.",
        "Green fruit, old patience.",
        "Your hour sweetens me.",
        "I hold one quiet olive.",
        "Still sour. Still proud.",
        "A fruit that remembers you.",
        "I grow flavor slowly.",
        "Warm skin, green heart.",
        "I practice being harvest.",
        "This olive is our draft.",
        "I keep it in the light.",
        "A little weight, new joy.",
        "Fruit after all that wait.",
        "I won't rush the oil.",
        "Stay. I'll ripen kinder.",
        "A green coin of time.",
        "I grew something to share.",
        "Tender, not ready, real.",
        "I cup this olive gently.",
        "The fruit likes your voice.",
        "I learned a heavier joy.",
        "A small olive, big thanks.",
        "I hold summer in a bead.",
        "Green, and a bit shy.",
        "This fruit is from your days.",
        "I grow it without hurry.",
        "A promise you can taste later.",
        "I keep the pit of hope.",
        "Come see, don't squeeze.",
        "I made food from focus.",
        "Tender fruit, careful hands.",
        "I ripen on your hours.",
        "A green gift, unfinished.",
        "I wait with this olive.",
        "Still young. Already ours.",
        "I grew a quiet harvest.",
        "One fruit, many mornings.",
        "I hold it like a secret.",
        "This olive says thank you.",
    },
    {
        "A crown of olives today.",
        "Fruit all around. Thank you.",
        "The crown is getting heavy.",
        "I grew a small harvest.",
        "I wear what we made.",
        "Olives in a quiet ring.",
        "A crown, not a show.",
        "I bow a little under fruit.",
        "Harvest sits on my head.",
        "Your years are in this crown.",
        "I hold many small suns.",
        "A full twig, calm pride.",
        "I grew enough to share.",
        "The crown remembers you.",
        "Olives like a soft yes.",
        "I wear patience out loud.",
        "A harvest you can count.",
        "I keep them for later oil.",
        "Heavy, and grateful.",
        "This crown is our work.",
        "I don't drop a single one.",
        "Stay. The harvest likes you.",
        "A ring of green gold.",
        "I grew a generous day.",
        "Fruit enough for two.",
        "I bow, I don't break.",
        "The crown came slowly.",
        "I hold a season up here.",
        "Olives, then more olives.",
        "I made a quiet feast.",
        "Your focus is the crown.",
        "I wear the long wait.",
        "A harvest without hurry.",
        "I keep them cool and kind.",
        "Come walk under this.",
        "I grew a full hello.",
        "The crown is not vanity.",
        "It is time, made round.",
        "I hold plenty, still green.",
        "A heavy thanks today.",
        "I share if you stay.",
        "Olives clustered like hours.",
        "I grew a small abundance.",
        "This is my fruit season.",
        "I keep the crown steady.",
        "A harvest that knows you.",
        "I bow to the work.",
        "Full branches, calm heart.",
        "I wear what you gave.",
        "This crown is ours.",
    },
    {
        "Old roots. Still with you.",
        "Years pass. I stay green.",
        "Deep roots, quiet joy.",
        "Timeless, as long as you come.",
        "I have seen many mornings.",
        "The years live under me.",
        "I am slow, and sure.",
        "Old wood, new thanks.",
        "I keep the long memory.",
        "Roots like old letters.",
        "I outlast the hurry.",
        "Still here. Still yours.",
        "I grow in decades now.",
        "A quiet forever, almost.",
        "I remember every hour.",
        "Deep, and not done.",
        "I hold the hill together.",
        "Time sits in my bark.",
        "I stay if you stay.",
        "Old olive, young care.",
        "I learned how to last.",
        "The roots go past worry.",
        "I am a long yes.",
        "Years, then your face.",
        "I keep shade for later you.",
        "Timeless is just daily care.",
        "I do not rush the end.",
        "Deep green, deeper calm.",
        "I have room for more years.",
        "Come. I kept your place.",
        "I root through dry days.",
        "An old friend in leaves.",
        "I measure time in rings.",
        "Still growing, on purpose.",
        "I hold yesterday gently.",
        "The long story continues.",
        "I am not finished yet.",
        "Roots, then another spring.",
        "I keep the old light.",
        "A century starts like this.",
        "I stay green for you.",
        "Deep roots, open door.",
        "I learned the word remain.",
        "Old, and still surprised.",
        "I grow slower, truer.",
        "This is the long season.",
        "I keep watch in winter.",
        "Your days became my age.",
        "I am here. That's the gift.",
        "Timeless, if we both stay.",
    },
};

static const char * const keep_talk_wither[KEEP_LEVEL_COUNT][KEEP_TALK_PER_LEVEL] = {
    {
        "I'm dry. I need your focus.",
        "The soil went too quiet.",
        "I barely made it out. Stay.",
        "One task, and I'll stand.",
        "I cracked the dirt. Then thirst.",
        "Don't leave a sprout alone.",
        "The light missed me today.",
        "I shrink without your hour.",
        "A dry hello from below.",
        "I need one small task.",
        "The soil feels like stone.",
        "I popped up, then wilted.",
        "Come back to this crack.",
        "I can't drink silence.",
        "A sprout asking for time.",
        "I fade in an empty day.",
        "Don't let me go back under.",
        "I need your morning.",
        "Dry tip, small voice.",
        "I waited. You didn't come.",
        "The first day feels long.",
        "I curl without care.",
        "One focus keeps me up.",
        "I am new, and already tired.",
        "The dust is winning.",
        "Stay. I just arrived.",
        "I lose green by evening.",
        "A thirsty first hello.",
        "I need a task, not shade.",
        "Don't forget this sprout.",
        "I opened. Then I dried.",
        "The hour would save me.",
        "I whisper from the dirt.",
        "Come before I fold in.",
        "I need you more than sun.",
        "A dry spark, still trying.",
        "I can't grow on zero.",
        "Please, one quiet focus.",
        "I slip back toward soil.",
        "This sprout is on pause.",
        "I need today's care.",
        "Empty day, empty root.",
        "I broke through for you.",
        "Don't leave me half-born.",
        "I wait in cracked earth.",
        "A small wilt, first stage.",
        "I need a done task.",
        "Come water me with time.",
        "I am here. Barely.",
        "One hour. I'll rise.",
    },
    {
        "My stem is bending in.",
        "I can't stand alone today.",
        "Don't leave this stem dark.",
        "One focus and I'll rise.",
        "I learned to stand. Then sag.",
        "The stem wants your hour.",
        "I lean too far now.",
        "Thin, and getting thinner.",
        "I lose my posture fast.",
        "A bent hello today.",
        "I need a stake of time.",
        "Don't let me fold.",
        "I stood. I miss you.",
        "The wind feels heavier.",
        "I droop without a task.",
        "Come hold this height.",
        "I can't stay upright alone.",
        "A slender wilt, quiet.",
        "I need today's focus.",
        "My spine is tired.",
        "I sway, then I sink.",
        "Don't leave me leaning.",
        "I practiced standing. Help.",
        "The stem remembers you.",
        "I shrink an inch by dusk.",
        "One task, I straighten.",
        "I am all stem, all thirst.",
        "Come back to this line.",
        "I lose the sky when you go.",
        "A bent olive, still hoping.",
        "I need you to stand.",
        "Empty hours bend me.",
        "I keep slipping down.",
        "Don't let the stem sleep.",
        "I wait in a slouch.",
        "Your time is my rod.",
        "I can't lift my head.",
        "A dry stem asking.",
        "I stood for you. Stay.",
        "The day is too long.",
        "I fold without a focus.",
        "Come, I'll stand again.",
        "I need one quiet hour.",
        "This thin green is fading.",
        "I lose height overnight.",
        "Don't forget the stem.",
        "I bend toward nothing.",
        "A wilted yes, unfinished.",
        "I need you more than rain.",
        "Help me stay upright.",
    },
    {
        "My first leaf is curling.",
        "This leaf wants your time.",
        "I'm folding in without you.",
        "Water me with a focus.",
        "I opened. Then I shut.",
        "The leaf edges go brown.",
        "I can't drink this silence.",
        "A curled hello today.",
        "I unfurled for nothing?",
        "Don't let the leaf close.",
        "I need today's hour.",
        "The green is rolling in.",
        "I lose my little sail.",
        "Come open me again.",
        "I practiced unfolding. Stay.",
        "A dry leaf, first one.",
        "I rustle, then I hush.",
        "The sun feels too sharp.",
        "I need shade and you.",
        "Don't leave this leaf.",
        "I fold like a tired hand.",
        "One task, I'll flatten.",
        "I lose dew by noon.",
        "The leaf remembers you.",
        "I opened to an empty day.",
        "Come before I crisp.",
        "I need a done todo.",
        "This green is retreating.",
        "I can't hold the light.",
        "A wilted page, blank.",
        "I curl at the edges.",
        "Don't let me close up.",
        "I wait in a tight roll.",
        "Your hour unrolls me.",
        "I am all leaf, all dry.",
        "Come back to this green.",
        "I lose my first shade.",
        "A small wilt on one leaf.",
        "I need you in the morning.",
        "The leaf asked. You didn't.",
        "I fold without a focus.",
        "Stay, I'll open again.",
        "I dry from tip to vein.",
        "Don't forget this leaf.",
        "I unfurled, then thirsted.",
        "A curled olive hello.",
        "I need one quiet task.",
        "The day closed me.",
        "I wait, rolled small.",
        "Open me with your time.",
    },
    {
        "This young branch misses you.",
        "The twig went droopy.",
        "Green wood dries fast.",
        "Come back. I'll branch again.",
        "I grew a limb. It sags.",
        "The fork feels too heavy.",
        "I can't hold both ways.",
        "A drooping hello today.",
        "Young wood wants your hour.",
        "Don't let this twig crack.",
        "I branch, then I wilt.",
        "The side road dried up.",
        "I need today's focus.",
        "A limp limb, still new.",
        "I lose my extra reach.",
        "Come hold this fork.",
        "I grew sideways. Help.",
        "The branch remembers you.",
        "I snap in spirit, not wood.",
        "Don't leave this twig.",
        "I can't shade a finger now.",
        "One task, I'll lift.",
        "I dry at the joint.",
        "Young wood, old thirst.",
        "I wait with a sag.",
        "Your time is this limb.",
        "I lose the second path.",
        "A wilted fork, quiet.",
        "I need a done task.",
        "The branch asks for you.",
        "I grew an arm. It aches.",
        "Come before I stiffen dry.",
        "I can't reach left or right.",
        "Don't forget this twig.",
        "I droop on both sides.",
        "A dry branch, still hoping.",
        "I need you in this wood.",
        "The day bent my fork.",
        "I wait, half-reaching.",
        "Stay. I'll lift the limb.",
        "I lose green in the twig.",
        "Empty hours thin the wood.",
        "I need one quiet hour.",
        "This branch is on pause.",
        "I grew it for you.",
        "Don't let the wood sleep.",
        "I sag without a focus.",
        "Come back to this fork.",
        "I am more tree, more thirst.",
        "Help this young branch.",
    },
    {
        "I stopped reaching today.",
        "The sky feels too far.",
        "No light, no lift.",
        "Pull me up with a task.",
        "I aimed up. Then I stalled.",
        "The climb needs your hour.",
        "I lose the noon.",
        "A low hello from high hopes.",
        "I can't chase the sun alone.",
        "Don't leave me mid-reach.",
        "I sag away from blue.",
        "The lift is gone.",
        "I need today's focus.",
        "Skyward, but stuck.",
        "I forget the word above.",
        "Come be my scaffold.",
        "I stop at yesterday's height.",
        "The light moved. I didn't.",
        "I droop from the top.",
        "Don't let me sink.",
        "I need a done todo.",
        "A wilted stretch, unfinished.",
        "I lose the high air.",
        "Your hour is the rope.",
        "I can't climb silence.",
        "Come before I slouch.",
        "I reach less each hour.",
        "The sky waits. I don't.",
        "I need you to rise.",
        "A dry arrow, pointing down.",
        "I stopped following the sun.",
        "Don't forget this climb.",
        "I wait below the light.",
        "One focus, I'll lift.",
        "I lose height by evening.",
        "Stay. I'll reach again.",
        "I am all up, all tired.",
        "The day pulled me down.",
        "I need one quiet task.",
        "Come back to the sky.",
        "I slouch in bright air.",
        "Empty hours ground me.",
        "I can't keep the stretch.",
        "A wilted yes to the sun.",
        "I need you more than noon.",
        "Don't leave me earthbound.",
        "I pause the climb.",
        "Help me look up.",
        "I wait with a bent crown.",
        "Pull me skyward again.",
    },
    {
        "Lush leaves are fading.",
        "So much green, going dull.",
        "The foliage went quiet.",
        "I need your hour back.",
        "My coat is thinning.",
        "Leaves whisper less.",
        "I lose the little room.",
        "A dull hello in green.",
        "The hush feels empty.",
        "Don't let the canopy thin.",
        "I rustle, then I stop.",
        "Shade is leaking away.",
        "I need today's focus.",
        "Silver backs turning pale.",
        "I can't keep this fullness.",
        "Come sit. I miss you.",
        "The leaves ask for time.",
        "I drop the cool air.",
        "A thinning tent today.",
        "Don't leave this green.",
        "I lose leaves in spirit.",
        "One task, I'll thicken.",
        "The wind finds holes.",
        "I wear less than yesterday.",
        "Your hour is this coat.",
        "I fade from the inside.",
        "Come before I go sparse.",
        "I need a done task.",
        "The foliage remembers you.",
        "I can't hide a secret now.",
        "A wilted woodland, small.",
        "I lose my leafy yes.",
        "Don't forget this shade.",
        "I wait under thin green.",
        "Stay. I'll fill again.",
        "I dry leaf by leaf.",
        "Empty hours thin me.",
        "I need one quiet hour.",
        "The canopy is tired.",
        "Come back to this hush.",
        "I rustle your name, faint.",
        "A dull coat, still here.",
        "I need you in the leaves.",
        "The day stole my thickness.",
        "I pause the lushness.",
        "Help me keep this green.",
        "I lose the pocket of dusk.",
        "Don't let me go bare.",
        "I wait, half-shaded.",
        "Bring the hour. I'll leaf.",
    },
    {
        "The bloom is waiting.",
        "Petals closed without you.",
        "A flower can wilt too.",
        "One focus, I'll bloom again.",
        "I opened. Then I shut.",
        "White going to paper.",
        "I can't hold this soft.",
        "A closed hello today.",
        "The bloom wants your hour.",
        "Don't let the petals dry.",
        "I flowered into silence.",
        "The bee of time is gone.",
        "I need today's focus.",
        "A pale wilt, quiet.",
        "I lose the white thought.",
        "Come before I drop.",
        "I bloomed for an empty day.",
        "The flower remembers you.",
        "I close without a task.",
        "Don't leave this bloom.",
        "I can't keep the hush sweet.",
        "One todo, I'll open.",
        "Petals fold like tired hands.",
        "I wait in a tight bud.",
        "Your hour is this flower.",
        "I fade at the edges.",
        "Come see, then stay.",
        "I need a done task.",
        "The bloom asked. You didn't.",
        "I lose my soft season.",
        "A wilted white, still here.",
        "Don't forget this petal.",
        "I dry in low voice.",
        "Stay. I'll flower again.",
        "I can't shout for care.",
        "Empty hours close me.",
        "I need one quiet hour.",
        "The day stole the bloom.",
        "I pause the spring.",
        "Help me keep this white.",
        "I wait, half-open.",
        "A gentle wilt, not gone.",
        "I need you more than bees.",
        "Don't let the flower sleep.",
        "I hold a tired petal.",
        "Come back to this bloom.",
        "I lose the pale yes.",
        "The hush went dry.",
        "I wait for your hour.",
        "Open me with a focus.",
    },
    {
        "Tender fruit is thirsty.",
        "The olive is shrinking in.",
        "Fruit waits. So do I.",
        "Don't let the green dry.",
        "I grew a bead. It wrinkles.",
        "The fruit wants your hour.",
        "I can't sweeten silence.",
        "A shrunk hello today.",
        "Don't pick. Don't leave.",
        "I lose the little weight.",
        "Young olive, old thirst.",
        "Come keep this warm.",
        "I need today's focus.",
        "A dry bead on the twig.",
        "I can't finish the flavor.",
        "The fruit remembers you.",
        "I wrinkle without a task.",
        "Don't forget this olive.",
        "I wait, still green, fading.",
        "One hour, I'll plump.",
        "I lose the promise.",
        "Your time is this fruit.",
        "I shrink by evening.",
        "Come before I harden dry.",
        "I need a done todo.",
        "A wilted gift, unfinished.",
        "I hold a thirsty globe.",
        "Don't let the pit win.",
        "I grew food. It waits.",
        "The day stole the juice.",
        "I pause the ripening.",
        "Stay. I'll swell again.",
        "I can't oil from zero.",
        "Empty hours thin the fruit.",
        "I need one quiet hour.",
        "Come back to this bead.",
        "I dry from skin to hope.",
        "A tender wilt, still here.",
        "I need you more than sun.",
        "Don't leave this olive.",
        "I wait with a small fruit.",
        "Help me keep it green.",
        "I lose the warm skin.",
        "The fruit asked for you.",
        "I wrinkle, I still hope.",
        "Come, I'll ripen kinder.",
        "I need today's care.",
        "A dry promise, not gone.",
        "Don't let harvest fail.",
        "Hold this olive with time.",
    },
    {
        "The crown feels too heavy.",
        "Olives wait on the branch.",
        "Harvest pauses without you.",
        "Come back to the crown.",
        "I wear fruit. It droops.",
        "The ring is tired.",
        "I can't hold this plenty.",
        "A heavy wilt today.",
        "The crown wants your hour.",
        "Don't let the cluster drop.",
        "I grew a harvest. Help.",
        "Plenty, and still thirsty.",
        "I need today's focus.",
        "Olives lean, then sag.",
        "I lose the quiet feast.",
        "Come walk under this.",
        "The crown remembers you.",
        "I bow too far now.",
        "Don't leave this harvest.",
        "I can't keep them cool.",
        "One task, I'll lift.",
        "I drop spirit, not fruit.",
        "Your years wait in the crown.",
        "I fade under my own weight.",
        "A wilted abundance, still here.",
        "I need a done todo.",
        "The feast went quiet.",
        "Don't forget this ring.",
        "I wait with full branches.",
        "Stay. I'll hold them.",
        "I lose the green gold.",
        "Empty hours sag the crown.",
        "I need one quiet hour.",
        "Come back to this plenty.",
        "I wear a tired harvest.",
        "Help me keep the cluster.",
        "I bow, I almost break.",
        "The day stole the lift.",
        "I pause the season.",
        "A heavy hello, thirsty.",
        "I need you under this.",
        "Don't let the crown sleep.",
        "I hold many, I need one you.",
        "The olives asked. Stay.",
        "I wilt at the top.",
        "Come, I'll share again.",
        "I need today's care.",
        "A full wilt, not empty.",
        "Don't waste this harvest.",
        "Hold the crown with time.",
    },
    {
        "Even old roots can wilt.",
        "Years of green, one dry day.",
        "I am old. I still need you.",
        "Timeless, but not without care.",
        "Deep roots, dry mouth.",
        "I last, I still thirst.",
        "The long memory is tired.",
        "An old wilt, quiet.",
        "Don't assume I'll stay.",
        "I need today's hour.",
        "Old wood dries too.",
        "Come back to the years.",
        "I kept your place. Empty.",
        "The rings remember thirst.",
        "I can't last on zero.",
        "A deep wilt, still here.",
        "I outlasted much. Not this.",
        "Don't leave the old friend.",
        "I need a done task.",
        "The roots ask for you.",
        "I stay. I sag.",
        "One focus, I'll last.",
        "I lose the long calm.",
        "Your days made my age. Stay.",
        "I wait through a dry year-day.",
        "Come sit in old shade.",
        "I am timeless only with you.",
        "Don't forget the deep green.",
        "I hold the hill, barely.",
        "A century still gets thirsty.",
        "I need one quiet hour.",
        "The old yes is whispering.",
        "I remain. I need care.",
        "Empty hours reach the roots.",
        "I pause the long season.",
        "Stay. I'll keep watch.",
        "I dry from the deep.",
        "An ancient wilt, not gone.",
        "I need you more than years.",
        "Don't let the roots sleep.",
        "I measured time. I miss you.",
        "Come back to this age.",
        "I last if you last.",
        "A deep hello, tired.",
        "Help me keep the green.",
        "I wait in old bark.",
        "The long story needs a page.",
        "I am here. Barely enough.",
        "Don't assume old means fine.",
        "One hour. I'll remain.",
    },
};

static struct {
    bool alive;
    bool leaving;
    bool events_bound;
    bool back_bound;
    bool displayed_withered;
    lv_obj_t *bound_scr;
    int last_ymd;
    int today_ymd;
} s_keep;

static void keep_clear_focus_box(lv_obj_t *obj)
{
    if (obj == NULL || !lv_obj_is_valid(obj)) {
        return;
    }
    lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_border_width(obj, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(obj, 0, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_shadow_width(obj, 0, LV_STATE_FOCUSED);
}

static void keep_keypad_group_reset(void)
{
    if (g == NULL) {
        g = lv_group_create();
    } else {
        lv_group_remove_all_objs(g);
        lv_group_set_edge_cb(g, NULL);
        lv_group_set_wrap(g, true);
    }
    lv_group_set_default(g);
    lv_indev_t *indev = NULL;
    while ((indev = lv_indev_get_next(indev))) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(indev, g);
        }
    }
}

static void keep_forget_screen(void)
{
    if (guider_ui.screen_keep != NULL &&
        !lv_obj_is_valid(guider_ui.screen_keep)) {
        guider_ui.screen_keep = NULL;
        guider_ui.screen_keep_img_bg = NULL;
        guider_ui.screen_keep_img_dialog = NULL;
        guider_ui.screen_keep_label_state = NULL;
        guider_ui.screen_keep_label_conversion = NULL;
        guider_ui.screen_keep_label_focustime = NULL;
        guider_ui.screen_keep_bar_focustime = NULL;
        guider_ui.screen_keep_img_process_icon = NULL;
    }
    s_keep.alive = false;
    s_keep.bound_scr = NULL;
    s_keep.events_bound = false;
    s_keep.back_bound = false;
    s_keep.leaving = false;
}

static bool keep_scr_ok(void)
{
    if (guider_ui.screen_keep == NULL ||
        !lv_obj_is_valid(guider_ui.screen_keep)) {
        keep_forget_screen();
        return false;
    }
    return true;
}

static bool keep_obj_ok(lv_obj_t *obj)
{
    return obj != NULL && lv_obj_is_valid(obj);
}

static int keep_ymd_pack(int y, int m, int d)
{
    if (y < 2000 || m < 1 || m > 12 || d < 1 || d > 31) {
        return 0;
    }
    return y * 10000 + m * 100 + d;
}

static int keep_ymd_to_days(int ymd)
{
    int y;
    int m;
    int d;

    if (ymd <= 0) {
        return 0;
    }
    y = ymd / 10000;
    m = (ymd / 100) % 100;
    d = ymd % 100;
    if (m < 3) {
        y--;
        m += 12;
    }
    return 365 * y + y / 4 - y / 100 + y / 400 + (153 * m - 457) / 5 + d;
}

#ifdef ESP_PLATFORM
static int keep_today_ymd(void)
{
    rx8130_time_t now;

    if (rx8130_get_time(I2C_NUM_0, &now) != ESP_OK) {
        return 0;
    }
    return keep_ymd_pack((int)now.year, (int)now.month, (int)now.date);
}

static void keep_nvs_save_ymd(int ymd)
{
    nvs_handle_t h;

    if (ymd <= 0) {
        return;
    }
    if (nvs_open(KEEP_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        return;
    }
    nvs_set_i32(h, KEEP_NVS_KEY, (int32_t)ymd);
    nvs_commit(h);
    nvs_close(h);
}

static int keep_nvs_load_ymd(void)
{
    nvs_handle_t h;
    int32_t stored = 0;

    if (nvs_open(KEEP_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        return 0;
    }
    if (nvs_get_i32(h, KEEP_NVS_KEY, &stored) != ESP_OK || stored <= 0) {
        nvs_close(h);
        return 0;
    }
    nvs_close(h);
    return (int)stored;
}
#endif

#ifdef ESP_PLATFORM
static void keep_cache_today(void)
{
    int today = keep_today_ymd();

    if (today > 0) {
        s_keep.today_ymd = today;
    }
}
#endif

static bool keep_is_withered(void)
{
    int today = s_keep.today_ymd;
    int last = s_keep.last_ymd;
    int idle;

    if (today <= 0 || last <= 0) {
        return false;
    }
    idle = keep_ymd_to_days(today) - keep_ymd_to_days(last);
    return idle >= KEEP_WITHER_DAYS;
}

static void keep_level_from_focus(int *level, int *span_cur,
                                 int *span_need, int *next_total)
{
    int i;
    int total = all_focus_time;
    int prev = 0;

    if (total < 0) {
        total = 0;
    }
    for (i = 0; i < KEEP_LEVEL_COUNT; i++) {
        int need = keep_level_hour[i] * 60;
        if (total < need) {
            *level = i + 1;
            *span_cur = total - prev;
            *span_need = need - prev;
            *next_total = need;
            return;
        }
        prev = need;
    }
    *level = KEEP_LEVEL_COUNT;
    *span_cur = 1;
    *span_need = 1;
    *next_total = keep_level_hour[KEEP_LEVEL_COUNT - 1] * 60;
}

static void keep_collect_plant_imgs(lv_obj_t **ok, lv_obj_t **wither)
{
    ok[0] = guider_ui.screen_keep_img_lv1;
    ok[1] = guider_ui.screen_keep_img_lv2;
    ok[2] = guider_ui.screen_keep_img_lv3;
    ok[3] = guider_ui.screen_keep_img_lv4;
    ok[4] = guider_ui.screen_keep_img_lv5;
    ok[5] = guider_ui.screen_keep_img_lv6;
    ok[6] = guider_ui.screen_keep_img_lv7;
    ok[7] = guider_ui.screen_keep_img_lv8;
    ok[8] = guider_ui.screen_keep_img_lv9;
    ok[9] = guider_ui.screen_keep_img_lv10;
    wither[0] = guider_ui.screen_keep_img_lv1_wither;
    wither[1] = guider_ui.screen_keep_img_lv2_wither;
    wither[2] = guider_ui.screen_keep_img_lv3_wither;
    wither[3] = guider_ui.screen_keep_img_lv4_wither;
    wither[4] = guider_ui.screen_keep_img_lv5_wither;
    wither[5] = guider_ui.screen_keep_img_lv6_wither;
    wither[6] = guider_ui.screen_keep_img_lv7_wither;
    wither[7] = guider_ui.screen_keep_img_lv8_wither;
    wither[8] = guider_ui.screen_keep_img_lv9_wither;
    wither[9] = guider_ui.screen_keep_img_lv10_wither;
}

static void keep_set_hidden(lv_obj_t *obj, bool hidden)
{
    if (!keep_obj_ok(obj)) {
        return;
    }
    if (hidden) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static uint32_t keep_rand_index(uint32_t n)
{
    if (n <= 1) {
        return 0;
    }
#ifdef ESP_PLATFORM
    return esp_random() % n;
#else
    return (uint32_t)rand() % n;
#endif
}

static const char *keep_pick_talk(int level, bool wilted)
{
    uint32_t idx;

    if (level < 1 || level > KEEP_LEVEL_COUNT) {
        level = 1;
    }
    idx = keep_rand_index(KEEP_TALK_PER_LEVEL);
    return wilted ? keep_talk_wither[level - 1][idx]
                  : keep_talk_ok[level - 1][idx];
}

static void keep_sync_process_icon(int percent)
{
    lv_obj_t *bar = guider_ui.screen_keep_bar_focustime;
    lv_obj_t *icon = guider_ui.screen_keep_img_process_icon;
    lv_coord_t bar_x;
    lv_coord_t bar_w;
    lv_coord_t icon_w;
    lv_coord_t travel;
    lv_coord_t x;

    if (!keep_obj_ok(bar) || !keep_obj_ok(icon)) {
        return;
    }
    if (percent < 0) {
        percent = 0;
    }
    if (percent > 100) {
        percent = 100;
    }
    bar_x = lv_obj_get_x(bar);
    bar_w = lv_obj_get_width(bar);
    icon_w = lv_obj_get_width(icon);
    if (icon_w < 1) {
        icon_w = 14;
    }
    travel = bar_w - icon_w;
    if (travel < 0) {
        travel = 0;
    }
    x = bar_x + (lv_coord_t)(travel * percent / 100);
    lv_obj_set_x(icon, x);
}

static void keep_refresh_ui(void)
{
    lv_obj_t *ok[KEEP_LEVEL_COUNT];
    lv_obj_t *wither[KEEP_LEVEL_COUNT];
    int level = 1;
    int span_cur = 0;
    int span_need = 180;
    int next_total = 180;
    int percent = 0;
    int shown;
    bool wilted;
    int i;

    if (!keep_scr_ok()) {
        return;
    }
    keep_collect_plant_imgs(ok, wither);
    keep_level_from_focus(&level, &span_cur, &span_need, &next_total);
    wilted = keep_is_withered();
    s_keep.displayed_withered = wilted;
    shown = all_focus_time < 0 ? 0 : all_focus_time;

    for (i = 0; i < KEEP_LEVEL_COUNT; i++) {
        bool show_ok = (!wilted && (i + 1) == level);
        bool show_w = (wilted && (i + 1) == level);

        keep_set_hidden(ok[i], !show_ok);
        keep_set_hidden(wither[i], !show_w);
        if (keep_obj_ok(ok[i])) {
            lv_obj_clear_flag(ok[i], LV_OBJ_FLAG_CLICKABLE);
        }
        if (keep_obj_ok(wither[i])) {
            lv_obj_clear_flag(wither[i], LV_OBJ_FLAG_CLICKABLE);
        }
    }

    if (level >= KEEP_LEVEL_COUNT && shown >= next_total) {
        percent = 100;
    } else if (span_need > 0) {
        percent = span_cur * 100 / span_need;
        if (percent > 100) {
            percent = 100;
        }
        if (percent < 0) {
            percent = 0;
        }
    }

    if (keep_obj_ok(guider_ui.screen_keep_bar_focustime)) {
        lv_bar_set_value(guider_ui.screen_keep_bar_focustime, percent, LV_ANIM_OFF);
    }
    keep_sync_process_icon(percent);
    if (keep_obj_ok(guider_ui.screen_keep_label_focustime)) {
        lv_label_set_text_fmt(guider_ui.screen_keep_label_focustime,
                              "%d/%d MIN", shown, next_total);
    }
    if (keep_obj_ok(guider_ui.screen_keep_label_state) &&
        level >= 1 && level <= KEEP_LEVEL_COUNT) {
        lv_label_set_text(guider_ui.screen_keep_label_state,
                          keep_state_names[level - 1]);
    }
    if (keep_obj_ok(guider_ui.screen_keep_label_conversion) &&
        level >= 1 && level <= KEEP_LEVEL_COUNT) {
        lv_label_set_text(guider_ui.screen_keep_label_conversion,
                          keep_pick_talk(level, wilted));
    }
}

static void keep_leave_to_main_async(void *unused)
{
    LV_UNUSED(unused);
    if (!keep_scr_ok()) {
        s_keep.leaving = false;
        return;
    }
    if (g != NULL) {
        lv_group_set_edge_cb(g, NULL);
        lv_group_set_wrap(g, true);
    }
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main,
                          guider_ui.screen_main_del, &guider_ui.screen_keep_del,
                          setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON,
                          100, 100, true, true);
}

static void keep_on_screen_delete(lv_event_t *e)
{
    lv_obj_t *target;

    if (lv_event_get_code(e) != LV_EVENT_DELETE) {
        return;
    }
    target = lv_event_get_target(e);
    if (guider_ui.screen_keep == target) {
        guider_ui.screen_keep = NULL;
        guider_ui.screen_keep_img_bg = NULL;
        guider_ui.screen_keep_img_dialog = NULL;
        guider_ui.screen_keep_label_state = NULL;
        guider_ui.screen_keep_label_conversion = NULL;
        guider_ui.screen_keep_label_focustime = NULL;
        guider_ui.screen_keep_bar_focustime = NULL;
        guider_ui.screen_keep_img_process_icon = NULL;
    }
    s_keep.alive = false;
    s_keep.bound_scr = NULL;
    s_keep.events_bound = false;
    s_keep.back_bound = false;
    s_keep.leaving = false;
}

static void keep_on_back(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    if (s_keep.leaving) {
        return;
    }
    if (!keep_scr_ok() || lv_scr_act() != guider_ui.screen_keep) {
        return;
    }
    s_keep.leaving = true;
    lv_async_call(keep_leave_to_main_async, NULL);
}

static void keep_bind_back_obj(lv_obj_t *obj)
{
    if (!keep_obj_ok(obj)) {
        return;
    }
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(obj, keep_on_back, LV_EVENT_CLICKED, NULL);
}

static void keep_bind_delete(void)
{
    if (!keep_obj_ok(guider_ui.screen_keep) || s_keep.events_bound) {
        return;
    }
    lv_obj_add_event_cb(guider_ui.screen_keep, keep_on_screen_delete,
                        LV_EVENT_DELETE, NULL);
    s_keep.events_bound = true;
}

void screen_keep_buttion_add(void)
{
    if (!keep_scr_ok() || lv_scr_act() != guider_ui.screen_keep) {
        return;
    }
    keep_bind_delete();
    keep_keypad_group_reset();
    lv_obj_add_flag(guider_ui.screen_keep, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(guider_ui.screen_keep, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    if (!s_keep.back_bound) {
        keep_bind_back_obj(guider_ui.screen_keep);
        keep_bind_back_obj(guider_ui.screen_keep_img_bg);
        keep_bind_back_obj(guider_ui.screen_keep_img_dialog);
        keep_bind_back_obj(guider_ui.screen_keep_label_state);
        keep_bind_back_obj(guider_ui.screen_keep_label_conversion);
        keep_bind_back_obj(guider_ui.screen_keep_label_focustime);
        s_keep.back_bound = true;
    }
    lv_group_add_obj(g, guider_ui.screen_keep);
    keep_clear_focus_box(guider_ui.screen_keep);
    group_focus_obj(guider_ui.screen_keep);
}

void screen_keep_custom_init(void)
{
    if (!keep_scr_ok() || s_keep.leaving) {
        return;
    }
    /* Bind DELETE before the fade delay ends so auto_del cannot leak the
     * pointer. Do not steal the shared keypad group until this screen is act. */
    keep_bind_delete();
    if (lv_scr_act() != guider_ui.screen_keep) {
        return;
    }
    lv_async_call_cancel(keep_leave_to_main_async, NULL);
    s_keep.alive = true;
    s_keep.bound_scr = guider_ui.screen_keep;
    keep_refresh_ui();
    screen_keep_buttion_add();
#ifdef ESP_PLATFORM
    ESP_LOGI("keep", "olive tree ready, focus=%d wither=%d",
             all_focus_time, (int)keep_is_withered());
#endif
}

void keep_machine_init(void)
{
#ifdef ESP_PLATFORM
    s_keep.last_ymd = keep_nvs_load_ymd();
    keep_cache_today();
#endif
}

void keep_note_activity(void)
{
#ifdef ESP_PLATFORM
    keep_cache_today();
    if (s_keep.today_ymd <= 0) {
        return;
    }
    if (s_keep.today_ymd == s_keep.last_ymd) {
        return;
    }
    s_keep.last_ymd = s_keep.today_ymd;
    keep_nvs_save_ymd(s_keep.last_ymd);
#endif
}

void keep_machine_refresh_today(void)
{
#ifdef ESP_PLATFORM
    keep_cache_today();
#endif
}

void keep_machine_tick(void)
{
#ifdef ESP_PLATFORM
    static TickType_t last_tick;
    TickType_t now = xTaskGetTickCount();

    if (s_keep.today_ymd > 0 &&
        (now - last_tick) < pdMS_TO_TICKS(30000)) {
        return;
    }
    last_tick = now;
    keep_cache_today();
#endif
}

void keep_machine_poll(void)
{
    lv_obj_t *act = lv_scr_act();

    if (!keep_scr_ok() || act == NULL || act != guider_ui.screen_keep) {
        if (!s_keep.leaving) {
            s_keep.alive = false;
            s_keep.bound_scr = NULL;
        }
        return;
    }
    if (s_keep.leaving) {
        return;
    }
    if (s_keep.alive && s_keep.bound_scr == act) {
        /* RTC sync or a day change can alter the plant while this page stays open.
         * poll runs under the LVGL mutex; cache updates never touch widgets. */
        if (s_keep.displayed_withered != keep_is_withered()) {
            keep_refresh_ui();
        }
        return;
    }
    screen_keep_custom_init();
}
