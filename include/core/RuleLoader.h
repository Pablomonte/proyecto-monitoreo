#pragma once
#include "ControlMediator.h"
#include "Rule.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

/**
 * RuleLoader — carga reglas desde SPIFFS/rules.json y las registra
 *              en el ControlMediator.
 *
 * Formato JSON soportado:
 * {
 *   "rules": [
 *     {
 *       "expr": {
 *         "op": "AND",
 *         "left":  { "sensor": {"device":0,"id":1}, "cond":"LT", "value":30 },
 *         "right": { "sensor": {"device":0,"id":0}, "cond":"GT", "value":50 }
 *       },
 *       "actuator": 1,
 *       "state": true,
 *       "priority": 2,
 *       "duration_ms": 5000
 *     }
 *   ]
 * }
 *
 * Operadores "op"/"cond": "GT", "LT", "EQ", "GTE", "LTE"
 * Operadores de árbol "op": "AND", "OR" (nodos internos)
 *                           cualquier otro → hoja LEAF (se ignora op)
 *
 * Uso:
 *   RuleLoader::load(mediator);         // en setup()
 *   RuleLoader::load(mediator, true);   // reload (limpia reglas previas)
 */
class RuleLoader {
public:
    /**
     * Cargar reglas desde /rules.json en SPIFFS.
     * @param mediator  Instancia del mediador a poblar.
     * @param clearFirst Si true, llama mediator.clearRules() antes de cargar.
     * @return Número de reglas cargadas, o -1 si falla la apertura del archivo.
     */
    static int load(ControlMediator& mediator, bool clearFirst = true) {
        if (!SPIFFS.exists("/rules.json")) {
            return 0;  // sin reglas — arranque limpio
        }

        File f = SPIFFS.open("/rules.json", "r");
        if (!f) return -1;

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, f);
        f.close();

        if (err) return -1;

        if (clearFirst) mediator.clearRules();

        JsonArray rules = doc["rules"].as<JsonArray>();
        if (rules.isNull()) return 0;

        int loaded = 0;
        for (JsonObject ruleObj : rules) {
            if (_loadRule(mediator, ruleObj)) loaded++;
        }
        return loaded;
    }

private:
    // Parsea una regla completa y la agrega al mediador
    static bool _loadRule(ControlMediator& mediator, JsonObject obj) {
        if (!obj.containsKey("expr")) return false;

        uint8_t rootIdx = _loadExpr(mediator, obj["expr"]);
        if (rootIdx == 0xFF) return false;

        Rule rule;
        rule.rootExprIdx  = rootIdx;
        rule.actuatorId   = obj["actuator"]    | (uint8_t)0;
        rule.triggerState = obj["state"]        | true;
        rule.priority     = obj["priority"]     | (uint8_t)2;
        rule.durationMs   = obj["duration_ms"]  | (uint32_t)0;

        mediator.addRule(rule);
        return true;
    }

    // Parsea recursivamente un nodo de expresión; retorna índice en pool o 0xFF
    static uint8_t _loadExpr(ControlMediator& mediator, JsonVariant node) {
        if (node.isNull()) return 0xFF;

        const char* op = node["op"] | "";

        // Nodo AND / OR
        if (strcmp(op, "AND") == 0 || strcmp(op, "OR") == 0) {
            uint8_t leftIdx  = _loadExpr(mediator, node["left"]);
            uint8_t rightIdx = _loadExpr(mediator, node["right"]);
            if (leftIdx == 0xFF || rightIdx == 0xFF) return 0xFF;

            RuleExpr expr;
            expr.type     = (strcmp(op, "AND") == 0) ? ExprType::AND : ExprType::OR;
            expr.leftIdx  = leftIdx;
            expr.rightIdx = rightIdx;
            // cond sin usar en nodos internos
            expr.cond = {{0, 0}, CondOp::GT, 0.0f};
            return mediator.addExpr(expr);
        }

        // Nodo LEAF
        JsonObject sensorObj = node["sensor"];
        if (sensorObj.isNull()) return 0xFF;

        const char* condStr = node["cond"] | "GT";

        RuleExpr expr;
        expr.type = ExprType::LEAF;
        expr.cond.key.deviceId  = sensorObj["device"]  | (uint8_t)0;
        expr.cond.key.sensorId  = sensorObj["id"]       | (uint8_t)0;
        expr.cond.threshold     = node["value"]          | 0.0f;
        expr.cond.op            = _parseCondOp(condStr);
        expr.leftIdx  = 0;
        expr.rightIdx = 0;
        return mediator.addExpr(expr);
    }

    static CondOp _parseCondOp(const char* s) {
        if (!s) return CondOp::GT;
        if (strcmp(s, "LT")  == 0) return CondOp::LT;
        if (strcmp(s, "EQ")  == 0) return CondOp::EQ;
        if (strcmp(s, "GTE") == 0) return CondOp::GTE;
        if (strcmp(s, "LTE") == 0) return CondOp::LTE;
        return CondOp::GT;
    }
};
