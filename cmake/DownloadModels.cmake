include_guard(GLOBAL)

function(retinify_download_models RELEASE_API_URL DESTINATION_DIR OUTPUT_VAR)
    set(release_json_path "${CMAKE_BINARY_DIR}/retinify_models_release.json")

    file(DOWNLOAD
        "${RELEASE_API_URL}"
        "${release_json_path}"
        TLS_VERIFY ON
        STATUS status
    )
    list(GET status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download release metadata: ${status}")
    endif()

    file(READ "${release_json_path}" release_json_content)

    string(JSON assets_length LENGTH "${release_json_content}" assets)
    if(assets_length EQUAL 0)
        message(FATAL_ERROR "No assets found in release metadata")
    endif()
    math(EXPR assets_last_index "${assets_length} - 1")

    file(MAKE_DIRECTORY "${DESTINATION_DIR}")

    set(downloaded_assets "")
    foreach(i RANGE 0 ${assets_last_index})
        string(JSON asset_name GET "${release_json_content}" assets ${i} name)
        string(JSON asset_digest GET "${release_json_content}" assets ${i} digest)
        string(JSON asset_download_url GET "${release_json_content}" assets ${i} browser_download_url)
        string(REGEX REPLACE "^sha256:" "" asset_hash_hex "${asset_digest}")

        if(asset_name STREQUAL "" OR asset_hash_hex STREQUAL "" OR asset_download_url STREQUAL "")
            message(FATAL_ERROR "Asset metadata is incomplete for index ${i}")
        endif()

        set(destination_path "${DESTINATION_DIR}/${asset_name}")
        set(expected_hash "SHA256=${asset_hash_hex}")

        file(DOWNLOAD
            "${asset_download_url}"
            "${destination_path}"
            EXPECTED_HASH "${expected_hash}"
            TLS_VERIFY ON
            STATUS status
        )
        list(GET status 0 status_code)
        if(NOT status_code EQUAL 0)
            message(FATAL_ERROR "Failed to download ${asset_name}: ${status}")
        endif()

        list(APPEND downloaded_assets "${destination_path}")
    endforeach()

    set(${OUTPUT_VAR} "${downloaded_assets}" PARENT_SCOPE)
endfunction()
