/*
 * config.h
 * -------------
 * Purpose: Configuration file for the vitaTrak application.
 * Notes  : You will need to bring your own API keys for ModArchive.
 *
 */

#ifndef VITATRAK_CONFIG_H
#define VITATRAK_CONFIG_H

// ModArchive support, requires API keys - disable to compile without ModArchive support
// 0 = disabled, 1 = enabled
#define VITATRAK_MODARCHIVE_SUPPORT 0
///////////////////////////////////////////
#ifdef VITATRAK_MODARCHIVE_SUPPORT
// You need to bring your own API keys for ModArchive support.
// e.g. "1234567890a1234567890af"
#define VITATRAK_MODARCHIVE_API_KEY "YOUR_API_KEY_HERE"

// Depending on what level your API key is, you may get additional information from ModArchive
//      that can be used to control more search result parameters.
// e.g. "3" or "5" (see https://modarchive.org/index.php?xml-api)
#define VITATRAK_MODARCHIVE_API_LEVEL 0

#endif // VITATRAK_MODARCHIVE_SUPPORT
#endif // VITATRAK_CONFIG_H
