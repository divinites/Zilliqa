import sendJsonRpcRequest from "../../helper/JsonRpcHelper";
import {assert} from "chai";
import logDebug from "../../helper/DebugHelper";

const METHOD = "net_listening";

describe("Calling " + METHOD, function () {
  it("should return the network listening state", async function () {
    await sendJsonRpcRequest(METHOD, 1, [], (result, status) => {
      logDebug(result);

      assert.equal(status, 200, "has status code");
      assert.property(result, "result", result.error ? result.error.message : "error");
      assert.isBoolean(result.result, "can be converted to a boolean");

      const expectedNetListing = true;
      assert.equal(
        result.result,
        expectedNetListing,
        "has result:" + result.result + ", expected net_listening:" + expectedNetListing
      );
    });
  });
});
