const booleanPointInPolygon = require('@turf/boolean-point-in-polygon').default;
const area = require('@turf/area').default;
const chroma = require('chroma-js');

const fetch = require('isomorphic-fetch');
const fs = require('fs');
const path = require('path');
const helpers = require('@turf/helpers');
 
const familyMedicinePath = `${__dirname}/../../data/json/national/family_medicine.json`;
const outPath = `${__dirname}/../../data/json/national/uats_with_family_medicine_weights.json`;

const medicalData = JSON.parse(fs.readFileSync(familyMedicinePath, 'utf8'));

fetch(`https://cdn.jsdelivr.net/gh/civicnet/geojson-romania@0.1.0/generated/uats.json`)
    .then(res => res.json())
    .then(json => {
        let data = json.map(uat => {
            const weight = medicalData.reduce((acc, fmData) => {
                const isContained = booleanPointInPolygon(
                    helpers.point([
                        fmData.lng, 
                        fmData.lat,
                    ]),
                    uat.geometry,
                );
                if (isContained) {
                    return acc + 1;
                } 

                return acc;
            }, 0);
            
            const calculatedArea = area(uat.geometry) / 1000;
            const popDensity = uat.properties.pop2015 / calculatedArea;
            const medDensity = weight !== 0
                ? weight / (uat.properties.pop2015 / 1000)
                : 0;

            /* if (weight > 0) {
                console.log(`Found ${uat.properties.name} with weight ${weight}`);
            } */

            return {
                ...uat,
                properties: {
                    ...uat.properties,
                    medicalWeight: weight,
                    popDensity,
                    medDensity,
                    area: calculatedArea,
                }
            }
        });

        const popBreaks = chroma.limits(data.map(item => item.properties.popDensity), 'q', 3);
        const popScale = chroma.scale(['#FDE725', '#73D055'])
            .classes(popBreaks);

        const medBreaks = chroma.limits(data.map(item => item.properties.medDensity), 'q', 3);
        const medScale = chroma.scale(['#404788', '#440154'])
            .classes(medBreaks);

        data = data.map(uat => ({
            ...uat,
            properties: {
                ...uat.properties,
                color: chroma.blend(
                    medScale(uat.properties.medDensity),
                    popScale(uat.properties.popDensity),
                    'darken'
                ).hex(),
            }
        }));

        fs.writeFile(outPath, JSON.stringify(data), 'utf8', (err) => {
            if (err) {
                console.log(`Error saving: ${err.message}`);
                return console.log(err);
            }
         
            console.log(`Weights saved to ${outPath}`);
        });
    });
