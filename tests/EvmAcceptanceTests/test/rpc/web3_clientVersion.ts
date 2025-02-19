import sendJsonRpcRequest from "../../helper/JsonRpcHelper";
import {assert} from "chai";
import hre from "hardhat";
import logDebug from "../../helper/DebugHelper";

const METHOD = "web3_clientVersion";

describe("Calling " + METHOD, function () {
  it("should return the web3 client version", async function () {
    await sendJsonRpcRequest(METHOD, 1, [], (result, status) => {
      logDebug(result);
      assert.equal(status, 200, "has status code");
      assert.property(result, "result", result.error ? result.error.message : "error");
      assert.isString(result.result, "is string");

      const expectedWeb3ClientVersion = hre.getWeb3ClientVersion();
      assert.equal(
        result.result,
        expectedWeb3ClientVersion,
        "has result:" + result.result + ", expected web3_clientVersion:" + expectedWeb3ClientVersion
      );
    });
  });
});
